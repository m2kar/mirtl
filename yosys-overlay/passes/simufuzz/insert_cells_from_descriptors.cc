#include "kernel/rtlil.h"
#include "kernel/utils.h"
#include "kernel/log.h"
#include "kernel/yosys.h"

USING_YOSYS_NAMESPACE

// @param inserted_cells: This is a return value.Initially just an empty vector of vectors
void insert_cells_from_descriptors(RTLIL::Module *module, std::vector<std::vector<RTLIL::Cell*>> &inserted_cells, std::vector<std::vector<std::string>> &cell_types, std::vector<std::vector<std::vector<size_t>>> &cell_dimensions, std::vector<std::vector<std::vector<size_t>>> &cell_params) {
    if (cell_types.size() != cell_dimensions.size())
        log_cmd_error("Simufuzz: The number of cell types and cell dimensions must be the same.\n");
    if (cell_types.size() != cell_params.size())
        log_cmd_error("Simufuzz: The number of cell types and cell params must be the same.\n");

    for (size_t subnet_id = 0; subnet_id < cell_types.size(); subnet_id++) {
        inserted_cells.push_back(std::vector<RTLIL::Cell*>());
        std::cout << "Simufuzz: Adding subnet " << subnet_id << ". Size: " << inserted_cells.size() << " / " << cell_types.size() << std::endl;
        if (cell_types[subnet_id].size() != cell_dimensions[subnet_id].size())
            log_cmd_error("Simufuzz: The number of cell types and cell dimensions must be the same in subnet %ld.\n", subnet_id);
        if (cell_types[subnet_id].size() != cell_params[subnet_id].size())
            log_cmd_error("Simufuzz: The number of cell types and cell params must be the same in subnet %ld.\n", subnet_id);

        for (size_t cell_id_in_arr = 0; cell_id_in_arr < cell_types[subnet_id].size(); cell_id_in_arr++) {
            std::stringstream ss;
            ss << "\\celloutsig_" << subnet_id << "_" << cell_id_in_arr << "z";
            std::string str = ss.str();

            if (cell_types[subnet_id][cell_id_in_arr] == "_ANDNOT_") {
                // log("Cell type: _ANDNOT_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAndnotGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_AND_") {
                // log("Cell type: _AND_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAndGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_AOI3_") {
                // log("Cell type: _AOI3_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][3] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAoi3Gate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_AOI4_") {
                // log("Cell type: _AOI4_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][3] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][4] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAoi4Gate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_BUF_") {
                // log("Cell type: _BUF_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addBufGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_FF_") {
                // log("Cell type: _FF_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addFfGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_MUX_") {
                // log("Cell type: _MUX_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][3] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addMuxGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_NAND_") {
                // log("Cell type: _NAND_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNandGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_NMUX_") {
                // log("Cell type: _NMUX_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][3] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNmuxGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_NOR_") {
                // log("Cell type: _NOR_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNorGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_NOT_") {
                // log("Cell type: _NOT_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNotGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_OAI3_") {
                // log("Cell type: _OAI3_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][3] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addOai3Gate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_OAI4_") {
                // log("Cell type: _OAI4_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][3] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][4] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addOai4Gate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_ORNOT_") {
                // log("Cell type: _ORNOT_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addOrnotGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_OR_") {
                // log("Cell type: _OR_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addOrGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_XOR_") {
                // log("Cell type: _XOR_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addXorGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "_XNOR_") {
                // log("Cell type: _XNOR_\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][0] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][1] == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr][2] == 1);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addXnorGate(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "add") {
                // log("Cell type: add\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAdd(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "adff") {
                // log("Cell type: adff\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);
                size_t main_dimension = cell_dimensions[subnet_id][cell_id_in_arr][2];
                // log("Simufuzz: Adding adff with dimensions: %ld, %ld, %ld, %ld\n", cell_dimensions[subnet_id][cell_id_in_arr][0], cell_dimensions[subnet_id][cell_id_in_arr][1], cell_dimensions[subnet_id][cell_id_in_arr][2], cell_dimensions[subnet_id][cell_id_in_arr][3]);
                inserted_cells[subnet_id].push_back(module->addAdff(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], main_dimension), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }
            // else if (cell_types[subnet_id][cell_id_in_arr] == "adffe") {
            //     log("Cell type: adffe\n");
            //     log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 4);
            //     log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);
            //     inserted_cells[subnet_id].push_back(module->addAdffe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][4]), cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2], cell_params[subnet_id][cell_id_in_arr][3]));
            // }
            else if (cell_types[subnet_id][cell_id_in_arr] == "and") {
                // log("Cell type: and\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAnd(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            // else if (cell_types[subnet_id][cell_id_in_arr] == "demux") {
            //     log("Cell type: demux\n");
            //     log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
            //     log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
            //     inserted_cells[subnet_id].push_back(module->addDemux(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            // }
            else if (cell_types[subnet_id][cell_id_in_arr] == "div") {
                // log("Cell type: div\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDiv(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "divfloor") {
                // log("Cell type: divfloor\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDivFloor(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "eq") {
                // log("Cell type: eq\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addEq(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            // else if (cell_types[subnet_id][cell_id_in_arr] == "equiv") {
            //     log("Cell type: equiv\n");
            //     log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
            //     log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
            //     inserted_cells[subnet_id].push_back(module->addEquiv(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            // }
            else if (cell_types[subnet_id][cell_id_in_arr] == "eqx") {
                // log("Cell type: eqx\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addEqx(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "ge") {
                // log("Cell type: ge\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addGe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "gt") {
                // log("Cell type: gt\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addGt(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "le") {
                // log("Cell type: le\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addLe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "logic_and") {
                // log("Cell type: logic_and\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addLogicAnd(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "logic_not") {
                // log("Cell type: logic_not\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addLogicNot(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "logic_or") {
                // log("Cell type: logic_or\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addLogicOr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "lt") {
                // log("Cell type: lt\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addLt(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            // else if (cell_types[subnet_id][cell_id_in_arr] == "macc") {
            //     log("Cell type: macc\n");
            //     log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
            //     log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
            //     inserted_cells[subnet_id].push_back(module->addLt(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            // }
            else if (cell_types[subnet_id][cell_id_in_arr] == "mod") {
                // log("Cell type: mod\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addMod(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "modfloor") {
                // log("Cell type: modfloor\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addModFloor(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "mul") {
                // log("Cell type: mul\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addMul(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "mux") {
                // log("Cell type: mux\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addMux(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "ne") {
                // log("Cell type: ne\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "neg") {
                // log("Cell type: neg\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNeg(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "nex") {
                // log("Cell type: nex\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNex(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "not") {
                // log("Cell type: not\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addNot(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "or") {
                // log("Cell type: or\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addOr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "pos") {
                // log("Cell type: pos\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addPos(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "pow") {
                // log("Cell type: pow\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addPow(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "reduce_and") {
                // log("Cell type: reduce_and\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addReduceAnd(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "reduce_bool") {
                // log("Cell type: reduce_bool\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addReduceBool(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "reduce_or") {
                // log("Cell type: reduce_or\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addReduceOr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "reduce_xnor") {
                // log("Cell type: reduce_xnor\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addReduceXnor(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "reduce_xor") {
                // log("Cell type: reduce_xor\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addReduceXor(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "shift") {
                // log("Cell type: shift\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addShift(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            // else if (cell_types[subnet_id][cell_id_in_arr] == "shiftx") {
            //     log("Cell type: shiftx\n");
            //     log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
            //     log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);
            //     inserted_cells[subnet_id].push_back(module->addShiftx(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            // }
            else if (cell_types[subnet_id][cell_id_in_arr] == "shl") {
                // log("Cell type: shl\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addShl(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "shr") {
                // log("Cell type: shr\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addShr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "slice") {
                // log("Cell type: slice\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSlice(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire, cell_params[subnet_id][cell_id_in_arr][0]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "sshl") {
                // log("Cell type: sshl\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSshl(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "sshr") {
                // log("Cell type: sshr\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSshr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "sub") {
                // log("Cell type: sub\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSub(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "xnor") {
                // log("Cell type: xnor\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addXnor(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "xor") {
                // log("Cell type: xor\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addXor(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire));
            }

            // Flip-flops
            else if (cell_types[subnet_id][cell_id_in_arr] == "adff") {
                // log("Cell type: adff\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAdff(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire, RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], outwidth), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "adffe") {
                // log("Cell type: adffe\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAdffe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire, RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], outwidth), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2], cell_params[subnet_id][cell_id_in_arr][3]));
				// inserted_cells[subnet_id][inserted_cells[subnet_id].size()-1]->setParam(ID::WIDTH, outwire->width);
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "aldff") {
                // log("Cell type: aldff\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 2);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAldff(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire, cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "aldffe") {
                // log("Cell type: aldffe\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 6);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][5];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAldffe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][4]), outwire, cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "dff") {
                // log("Cell type: dff\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDff(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire, cell_params[subnet_id][cell_id_in_arr][0]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "dffe") {
                // log("Cell type: dffe\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 2);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDffe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire, cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "dffsr") {
                // log("Cell type: dffsr\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDffsr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire, cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "dffsre") {
                // log("Cell type: dffsre\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 6);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][5];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDffsre(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][4]), outwire, cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2], cell_params[subnet_id][cell_id_in_arr][3]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "ff") {
                // log("Cell type: ff\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 0);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 2);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][1];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addFf(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), outwire));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "sdff") {
                // log("Cell type: sdff\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSdff(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire, RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], outwidth), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "sdffce") {
                // log("Cell type: sdffce\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSdffce(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire, RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], outwidth), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2], cell_params[subnet_id][cell_id_in_arr][3]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "sdffe") {
                // log("Cell type: sdffe\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 4);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addSdffe(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire, RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], outwidth), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2], cell_params[subnet_id][cell_id_in_arr][3]));
            }

            // Latches
            else if (cell_types[subnet_id][cell_id_in_arr] == "adlatch") {
                // log("Cell type: adlatch\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 4);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][3];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addAdlatch(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), outwire, RTLIL::Const(cell_params[subnet_id][cell_id_in_arr][0], outwidth), cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "dlatch") {
                // log("Cell type: dlatch\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 1);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 3);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][2];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDlatch(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), outwire, cell_params[subnet_id][cell_id_in_arr][0]));
            }
            else if (cell_types[subnet_id][cell_id_in_arr] == "dlatchsr") {
                // log("Cell type: dlatchsr\n");
                log_assert(cell_params[subnet_id][cell_id_in_arr].size() == 3);
                log_assert(cell_dimensions[subnet_id][cell_id_in_arr].size() == 5);

                size_t outwidth = cell_dimensions[subnet_id][cell_id_in_arr][4];
                RTLIL::Wire *outwire = module->addWire(IdString(str), outwidth);
                inserted_cells[subnet_id].push_back(module->addDlatchsr(NEW_ID, module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][0]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][1]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][2]), module->addWire(NEW_ID, cell_dimensions[subnet_id][cell_id_in_arr][3]), outwire, cell_params[subnet_id][cell_id_in_arr][0], cell_params[subnet_id][cell_id_in_arr][1], cell_params[subnet_id][cell_id_in_arr][2]));
            }

            else
                log_cmd_error("Simufuzz: Unsupported cell type: `%s`\n", cell_types[subnet_id][cell_id_in_arr].c_str());
        }
    }
}
