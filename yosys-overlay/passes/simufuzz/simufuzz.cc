/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Alberto Gonzalez <boqwxp@airmail.cc> & Flavien Solt <flsolt@ethz.ch>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

// For all new cells, add src=cell->get_src_attribute()

#include "kernel/log.h"
#include "kernel/register.h"
#include "kernel/rtlil.h"
#include "kernel/utils.h"
#include "kernel/yosys.h"
#include <fstream>
#include "json.hpp"

#include <iostream>
#include <unordered_map>
#include <string>

USING_YOSYS_NAMESPACE

extern void insert_cells_from_descriptors(RTLIL::Module *module, std::vector<std::vector<RTLIL::Cell*>> &inserted_cells, std::vector<std::vector<std::string>> &cell_types, std::vector<std::vector<std::vector<size_t>>> &cell_dimensions, std::vector<std::vector<std::vector<size_t>>> &cell_params);

PRIVATE_NAMESPACE_BEGIN

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

struct SimuFuzzWorker {
private:
	// Command line arguments.
	bool opt_verbose = false;
	std::string opt_json_path = "";

	RTLIL::Module *module = nullptr;

	void create_simufuzz_logic()
	{
		// Remove the dummy connection from the toplevel module to have an empty playground
		module->connections_.clear();

		using json = nlohmann::json;

		// Open and parse the JSON file
		if (!fileExists(opt_json_path))
			log_cmd_error("Simufuzz: The JSON file `%s` does not exist.\n", opt_json_path.c_str());

		std::ifstream file(opt_json_path.c_str());
		json data;
		file >> data;

		// Configure the module_in_wire and module_out_wire signal widths
		int in_width = data["in_width"];
		int out_width = data["out_width"];
		auto module_clkin_wire = module->wire("\\clkin_data");
		std::vector<std::string> clkin_ports_names = data["clkin_ports_names"];
		auto module_in_wire = module->wire("\\in_data");
		auto module_out_wire = module->wire("\\out_data");
		if (module_in_wire == nullptr)
			log_cmd_error("Simufuzz: Module `%s` has no input wire named `in_data`.\n", module->name.c_str());
		if (module_out_wire == nullptr)
			log_cmd_error("Simufuzz: Module `%s` has no output wire named `out_data`.\n", module->name.c_str());
		std::vector<std::vector<std::string>> cell_types = data["cell_types"];
		int num_subnets = cell_types.size();
		module_in_wire->width = in_width * num_subnets;
		module_out_wire->width = out_width * num_subnets;

		// Compute the width of the clkin input wire
		std::vector<int> clkin_ports_widths = data["clkin_ports_widths"];
		std::vector<int> clkin_ports_offsets;
		uint32_t clkin_width = 0;
		if (clkin_ports_names.size() == 0) {
			clkin_width = 1;
		} else {
			std::vector<int> clkin_ports_widths = data["clkin_ports_widths"];
			if (clkin_ports_names.size() != clkin_ports_widths.size())
				log_cmd_error("Simufuzz: The number of clkin ports names and widths must be the same.\n");
			for (size_t i = 0; i < clkin_ports_names.size(); ++i) {
				clkin_ports_offsets.push_back(clkin_width);
				std::string clkin_port_name = clkin_ports_names[i];
				int clkin_port_width = clkin_ports_widths[i];
				// Round up to the next multiple of 32
				clkin_width += 32*((clkin_port_width + 31) / 32);
			}
		}
		module_clkin_wire->width = clkin_width;

		// The clkin input signals
		std::unordered_map<std::string, RTLIL::Wire*> clkin_input_wires;
		if (clkin_ports_names.size() != clkin_ports_widths.size())
			log_cmd_error("Simufuzz: The number of clkin ports names and widths must be the same.\n");
		for (size_t i = 0; i < clkin_ports_names.size(); ++i) {
			std::string clkin_port_name = clkin_ports_names[i];
			int clkin_port_width = clkin_ports_widths[i];
			RTLIL::Wire* clkin_wire = module->wire("\\" + clkin_port_name);
			if (clkin_wire == nullptr)
				log_cmd_error("Simufuzz: Module `%s` has no input wire named `%s`.\n", module->name.c_str(), clkin_port_name.c_str());
			clkin_wire->width = clkin_port_width;
			clkin_input_wires[clkin_port_name] = clkin_wire;

			RTLIL::SigSpec lhs = RTLIL::SigSpec(clkin_wire);
			RTLIL::SigSpec rhs = RTLIL::SigSpec(module_clkin_wire).extract(clkin_ports_offsets[i], clkin_port_width);
			module->connect(lhs, rhs);
		}
		std::cout << "Simufuzz: clkin_ports_names.size(): " << clkin_ports_names.size() << std::endl;

		// Create the new cells
		std::vector<std::vector<RTLIL::Cell*>> inserted_cells;
		std::vector<std::vector<std::vector<size_t>>> cell_dimensions = data["cell_dimensions"];
		std::vector<std::vector<std::vector<size_t>>> cell_params = data["cell_params"];

		if (cell_types.size() != cell_dimensions.size()) {
			log("Simufuzz: The number of cell types and cell dimensions must be the same.\n");
			log_assert(false);
		}
		insert_cells_from_descriptors(module, inserted_cells, cell_types, cell_dimensions, cell_params);

		// Check that all the cells have been inserted
		if (cell_types.size() != inserted_cells.size())
	        log_cmd_error("Simufuzz: The number of above cell types (%ld) and inserted cells (%ld) must be the same.\n", cell_types.size(), inserted_cells.size());

		for (size_t subnet_id = 0; subnet_id < inserted_cells.size(); subnet_id++) {
			if (cell_types[subnet_id].size() != inserted_cells[subnet_id].size()) {
				log_cmd_error("Simufuzz: The number of below cell types (%ld) and inserted cells (%ld) for subnet_id %ld must be the same.\n", cell_types[subnet_id].size(), inserted_cells[subnet_id].size(), subnet_id);
			}
		}

		// for (size_t subnet_id = 0; subnet_id < inserted_cells.size(); subnet_id++) {
		// 	for (size_t cell_type_id = 0; cell_type_id < cell_types[subnet_id].size(); cell_type_id++) {
		// 		std::cout << "Simufuzz: subnet_id: " << subnet_id << ", cell_type_id: " << cell_type_id << ", cell_types[subnet_id][cell_type_id]: " << cell_types[subnet_id][cell_type_id] << ", cell_dimensions[subnet_id][cell_type_id].size(): " << cell_dimensions[subnet_id][cell_type_id].size() << ", cell_params[subnet_id][cell_type_id].size(): " << cell_params[subnet_id][cell_type_id].size() << std::endl;
		// 	}
		// }

		// for (size_t subnet_id = 0; subnet_id < inserted_cells.size(); subnet_id++) {
		// 	std::cout << "Simufuzz: subnet_id: " << subnet_id << ", inserted_cells[" << subnet_id << "].size(): " << inserted_cells[subnet_id].size() << std::endl;
		// }

		// Make the connections
		// The int cannot be a size_t because we want them to be able to be -1
		std::vector<std::tuple<int, int, std::string, size_t, int, int, std::string, size_t, size_t>> connection_descriptors = data["connections"];
		// Add the probe output port if any
		RTLIL::Wire* module_probe_wire = nullptr;
		if (data.contains("probe_width")) {
			int probe_width = data["probe_width"];
			module_probe_wire = module->wire("\\probe_data");
			if (module_probe_wire == nullptr)
				log_cmd_error("Simufuzz: Module `%s` has no output wire named `probe_data`.\n", module->name.c_str());
			module_probe_wire->width = probe_width > 65 ? probe_width : 65;
		}
		size_t connection_id = -1;
		for (auto connection_descriptor: connection_descriptors) {
			connection_id++;
			int dst_subnet_id = std::get<0>(connection_descriptor);
			int dst_cell_index = std::get<1>(connection_descriptor);
			std::string dst_port_name = std::get<2>(connection_descriptor);
			size_t dst_port_offset = std::get<3>(connection_descriptor);
			int src_subnet_id = std::get<4>(connection_descriptor);
			int src_cell_index = std::get<5>(connection_descriptor);
			std::string src_port_name = std::get<6>(connection_descriptor);
			size_t src_port_offset = std::get<7>(connection_descriptor);
			size_t port_width = std::get<8>(connection_descriptor);
			bool is_clkin = false;

			// std::cout << "Simufuzz: dst_subnet_id: " << dst_subnet_id << ", dst_cell_index: " << dst_cell_index << ", dst_port_name: " << dst_port_name << ", dst_port_offset: " << dst_port_offset << ", src_subnet_id: " << src_subnet_id << ", src_cell_index: " << src_cell_index << ", src_port_name: " << src_port_name << ", src_port_offset: " << src_port_offset << ", port_width: " << port_width << std::endl;

			RTLIL::SigSpec dst_sigspec;
			RTLIL::SigSpec src_sigspec;
			// log("Simufuzz: dst_cell_index: %d, src_cell_index: %d\n", dst_cell_index, src_cell_index);
			if (dst_cell_index == -1) {
				// The destination is the toplevel module
				if (dst_port_name != "O" && dst_port_name != "P")
					log_cmd_error("Simufuzz: The toplevel module signal must be O or P. Instead, got `%s`.\n", dst_port_name.c_str());
				if (dst_port_name == "O")
					dst_sigspec = module_out_wire;
				else if (dst_port_name == "P")
					dst_sigspec = module_probe_wire;
				else {
					log("Simufuzz: Unexpected destination port name `%s`.\n", dst_port_name.c_str());
					log_assert(false);
				}
			} else {
				if ((size_t)dst_cell_index >= inserted_cells[dst_subnet_id].size())
					log_cmd_error("Simufuzz: The destination cell index `%d` is out of bounds (max: %ld) for connection id `%zu`, dst_subnet_id `%d`, dst_cell_index `%d`, dst_port_name `%s`, dst_port_offset `%lu`, src_subnet_id `%d`, src_cell_index `%d`, src_port_name `%s`, src_port_offset `%lu`, port_width `%lu`.\n", dst_cell_index, inserted_cells[dst_subnet_id].size()-1, connection_id, dst_subnet_id, dst_cell_index, dst_port_name.c_str(), dst_port_offset, src_subnet_id, src_cell_index, src_port_name.c_str(), src_port_offset, port_width);
				// The destination is a cell
				RTLIL::Cell *dst_cell = inserted_cells[dst_subnet_id][dst_cell_index];
				// log("Simufuzz: Cell type: %s, Port name: %s, formatted %s\n", dst_cell->type.c_str(), dst_port_name.c_str(), ("\\" + dst_port_name).c_str());
				if (!dst_cell->hasPort(("\\" + dst_port_name)))
					log_cmd_error("Simufuzz: The destination cell `%s` has no port `%s`.\n", dst_cell->type.c_str(), dst_port_name.c_str());
				dst_sigspec = dst_cell->getPort(("\\" + dst_port_name));
				// log("Simufuzz: dst_sigspec.width: %d, port: %s, is_wire: %d\n", dst_sigspec.size(), dst_port_name.c_str(), dst_sigspec.is_wire());
			}
			if (src_cell_index == -1) {
				// The source is the toplevel module
				if (src_port_name == "I")
					src_sigspec = module_in_wire;
				else if (src_port_name == "VCC") {
					src_sigspec = RTLIL::SigSpec(RTLIL::State::S1, port_width);
					if (src_port_offset != 0) {
						log("Simufuzz: The VCC source port offset is not 0. Setting it to 0.\n");
						src_port_offset = 0;
					}
				}
				else if (src_port_name == "GND") {
					src_sigspec = RTLIL::SigSpec(RTLIL::State::S0, port_width);
					if (src_port_offset != 0) {
						log("Simufuzz: The VCC source port offset is not 0. Setting it to 0.\n");
						src_port_offset = 0;
					}
				}
				else {
					// Check whether this is a clkin port

					// if (dst_subnet_id != -1) {
					// 	log("Simufuzz: Unexpected destination subnet id `%d`. Expected -1.\n", dst_subnet_id);
					// 	log_assert(false);
					// }

					for (auto clkin_input_wire: clkin_input_wires) {
						if (src_port_name == clkin_input_wire.first) {
							is_clkin = true;
							RTLIL::SigSpec src_sigspec_beforeoffset = clkin_input_wire.second;

							// Check that the offset is not too big
							if (src_port_offset + port_width > (size_t)src_sigspec_beforeoffset.size()) {
								log("Simufuzz: The clkin source port offset is too big. src_port_offset: %lu, port_width: %lu, sum: %lu, src_sigspec_beforeoffset.size(): %lu\n", src_port_offset, port_width, src_port_offset + port_width, (size_t)src_sigspec_beforeoffset.size());
								log_assert(false);
							}

							src_sigspec = src_sigspec_beforeoffset.extract(src_port_offset, port_width);
						}
					}

					if (!is_clkin) {
						log("Simufuzz: Unexpected source port name `%s`.\n", src_port_name.c_str());
						log_assert(false);
					}
				}
			} else {
				if ((size_t)src_cell_index >= inserted_cells[src_subnet_id].size())
					log_cmd_error("Simufuzz: The source cell index `%d` is out of bounds (max: %ld).\n", src_cell_index, inserted_cells[src_subnet_id].size()-1);

				// The source is a cell
				RTLIL::Cell *src_cell = inserted_cells[src_subnet_id][src_cell_index];
				// log("Simufuzz: Cell type: %s, Port name: %s, formatted %s, port width: %d\n, connection offset: %d, connection width: %d\n", src_cell->type.c_str(), src_port_name.c_str(), ("\\" + src_port_name).c_str(), src_cell->getPort(("\\" + src_port_name)).size(), src_port_offset, port_width);
				if (!src_cell->hasPort(("\\" + src_port_name)))
					log_cmd_error("Simufuzz: The source cell `%s` has no port `%s`.\n", src_cell->type.c_str(), src_port_name.c_str());
				src_sigspec = src_cell->getPort(("\\" + src_port_name));
			}

			// std::cout << "src_port_name: " << src_port_name << ", dst_port_name: " << dst_port_name << std::endl;
			// std::cout << "dst_port_offset: " << dst_port_offset << ", port_width: " << port_width << ", src_port_offset: " << src_port_offset << ", port_width: " << port_width << std::endl;
			// std::cout << "dst_sigspec.width: " << dst_sigspec.size() << ", src_sigspec.width: " << src_sigspec.size() << std::endl;

			if (!dst_sigspec.is_wire())
				log_cmd_error("Simufuzz: The destination signal is not a wire.\n");

			RTLIL::SigSpec dst_port = dst_sigspec.extract(dst_port_offset, port_width);
			RTLIL::SigSpec src_port = src_sigspec.extract(src_port_offset, port_width);

			// log("Simufuzz: Connecting dst_port_offset: %lu, port_width: %lu, src_port_offset: %lu, port_width: %lu\n", dst_port_offset, port_width, src_port_offset, port_width);

			module->connect(dst_port, src_port);
		}
		module->fixup_ports();
	}

public:
	SimuFuzzWorker(RTLIL::Module *_module, bool _opt_verbose, std::string _opt_json_path)
	{
		module = _module;
		opt_verbose = _opt_verbose;
		opt_json_path = _opt_json_path;
		log("Simufuzz: Starting to create logic for module `%s`.\n", module->name.c_str());
		create_simufuzz_logic();
		log("Simufuzz: Created logic for module `%s`.\n", module->name.c_str());
	}
};

struct SimuFuzzPass : public Pass {
	SimuFuzzPass() : Pass("simufuzz", "Create logic netlist from json representations.") {}

	void help() override
	{
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		log("\n");
		log("    simufuzz <command> [options] [selection]\n");
		log("\n");
		log("Creates a logic netlist from a JSON representation.\n");
		log("\n");
		log("Options:\n");
		log("\n");
		log("  -json-path\n");
		log("    The path to the JSON representation\n");
		log("\n");
		log("  -verbose\n");
		log("    Whether to make the pass verbose\n");
		log("\n");
	}

	void execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		bool opt_verbose = false;
		std::string opt_json_path = "";

		size_t argidx;
		for (argidx = 1; argidx < args.size(); argidx++) {
			if (args[argidx] == "-json-path") {
				opt_json_path = args[++argidx];
				continue;
			}
			if (args[argidx] == "-verbose") {
				opt_verbose = true;
				continue;
			}
			break;
		}
		extra_args(args, argidx, design);

		// Check whether some module is selected.
		if (opt_json_path.length() == 0)
			log_cmd_error("Simufuzz got an empty JSON path!\n");

		// Modules must be taken in inverted topological order to instrument the deepest modules first.
		// Taken from passes/techmap/flatten.cc
		// All this is not really necessary for now, given that we focus on a single module for now.
		TopoSort<RTLIL::Module *, IdString::compare_ptr_by_name<RTLIL::Module>> topo_modules;
		auto worklist = design->selected_modules();
		pool<RTLIL::IdString> non_top_modules;
		while (!worklist.empty()) {
			RTLIL::Module *module = *(worklist.begin());
			worklist.erase(worklist.begin());
			topo_modules.node(module);

			for (auto cell : module->selected_cells()) {
				RTLIL::Module *tpl = design->module(cell->type);
				if (tpl != nullptr) {
					if (topo_modules.get_database().count(tpl) == 0)
						worklist.push_back(tpl);
					topo_modules.edge(tpl, module);
					non_top_modules.insert(cell->type);
				}
			}
		}
		if (!topo_modules.sort())
			log_cmd_error("Recursive modules are not supported by Simufuzz.\n");

		if (GetSize(topo_modules.sorted) != 1)
			log_cmd_error("Simufuzz requires exactly one module to be selected. Got `%d` module(s) instead.\n", GetSize(topo_modules.sorted));

		// Run the worker on each module.
		for (auto i = 0; i < GetSize(topo_modules.sorted); ++i) {
			RTLIL::Module *module = topo_modules.sorted[i];
			SimuFuzzWorker(module, opt_verbose, opt_json_path);
		}
	}
} SimuFuzzPass;

PRIVATE_NAMESPACE_END
