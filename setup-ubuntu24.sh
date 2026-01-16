#!/bin/bash
# Copyright 2024 Flavien Solt, ETH Zurich.
# Licensed under the General Public License, Version 3.0, see LICENSE for details.
# SPDX-License-Identifier: GPL-3.0-only
#
# 在 Ubuntu 24.04 宿主机上配置 mirtl 运行环境
# 参考 docker/Dockerfile，但直接在宿主机上执行，不使用 Docker

set -e

# 检查是否为 root 用户
if [ "$EUID" -ne 0 ]; then 
    echo "请使用 sudo 运行此脚本"
    exit 1
fi

echo "=========================================="
echo "开始配置 mirtl 运行环境 (Ubuntu 24.04)"
echo "=========================================="

pushd ${PWD}
############################
# 安装系统依赖包
############################

echo "更新 apt 包列表..."
apt-get update

echo "安装基础工具..."
apt-get install -y curl gnupg apt-utils

echo "配置 SBT 仓库..."
# Ubuntu 24.04 使用新的 GPG key 管理方式
echo "deb https://repo.scala-sbt.org/scalasbt/debian /" | tee /etc/apt/sources.list.d/sbt.list
curl -sL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2EE0EA64E40A89B84B2DF73499E82A75642AC823" | gpg --dearmor | tee /etc/apt/trusted.gpg.d/sbt.gpg > /dev/null

echo "更新 apt 包列表..."
apt-get update

echo "安装开发工具和依赖..."
DEBIAN_FRONTEND=noninteractive apt-get install -y \
    autoconf automake autotools-dev libmpc-dev libmpfr-dev libgmp-dev gawk build-essential \
    bison flex texinfo gperf libtool patchutils bc zlib1g-dev git perl  make g++ libfl2 \
    libfl-dev zlib1g zlib1g-dev git autoconf flex bison gtkwave clang \
    tcl-dev libreadline-dev jq libexpat-dev device-tree-compiler vim \
    sbt \
    software-properties-common default-jdk default-jre gengetopt \
    patch diffstat texi2html subversion chrpath wget \
    libgtk-3-dev gettext \
    python3 python3-venv python3-pip python3-dev rsync libguestfs-tools expat universal-ctags \
    libexpat1-dev libusb-dev libncurses5-dev cmake gtkwave help2man libmpc-dev libmpfr-dev libgmp-dev gawk ninja-build libglib2.0-dev libslirp-dev \
    libc6-dev libffi-dev xz-utils gnupg netbase \
    libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev libboost-iostreams-dev

# 检查并安装 Python 3.12
echo "检查 Python 3.12..."
if ! command -v python3.12 &> /dev/null; then
    echo "Python 3.12 未找到，尝试从 deadsnakes PPA 安装..."
    add-apt-repository -y ppa:deadsnakes/ppa
    apt-get update -y
    apt-get install -y python3.12 python3.12-venv python3.12-dev
else
    echo "Python 3.12 已安装"
    # 确保安装开发包
    apt-get install -y python3.12-venv python3.12-dev || true
fi

############################
# 安装 MiRTL Yosys
############################

echo "=========================================="
echo "安装 MiRTL Yosys"
echo "=========================================="

# 检查 mirtl-yosys.tgz 文件是否存在
MIRTL_YOSYS_TGZ="/root/mirtl/docker/mirtl-yosys/mirtl-yosys.tgz"
if [ ! -f "$MIRTL_YOSYS_TGZ" ]; then
    echo "错误: 找不到 $MIRTL_YOSYS_TGZ"
    echo "请根据 Readme.md 的说明下载 mirtl-yosys.tgz 文件"
    echo "下载地址: https://berkeley.box.com/s/lv2jg9fzvwdlimo43i9naks4c1d5u3oh"
    exit 1
fi

echo "解压并编译 MiRTL Yosys..."
cp "$MIRTL_YOSYS_TGZ" /mirtl-yosys.tgz
cd /
tar zxf mirtl-yosys.tgz
cd /mirtl-yosys
make -j$(nproc)
make install
cd -

############################
# 安装 Verilator 5.022
############################

echo "=========================================="
echo "安装 Verilator 5.022"
echo "=========================================="

cd /tmp
if [ -d "verilator" ]; then
    rm -rf verilator
fi
git clone https://github.com/verilator/verilator
unset VERILATOR_ROOT
cd verilator
git checkout v5.022
autoconf
./configure
make -j$(nproc)
make install
cd /
rm -rf /tmp/verilator

############################
# 安装 Icarus Verilog v12_0
############################

echo "=========================================="
echo "安装 Icarus Verilog v12_0"
echo "=========================================="

cd /tmp
if [ -d "iverilog" ]; then
    rm -rf iverilog
fi
git clone https://github.com/steveicarus/iverilog
cd iverilog
git checkout 548010e36be386c3f68807df8323719e3ac24325
autoconf
./configure
make -j$(nproc)
make install
cd /
rm -rf /tmp/iverilog

############################
# 配置 fuzzer 环境
############################

echo "=========================================="
echo "配置 fuzzer 环境"
echo "=========================================="

echo "安装 Python 依赖..."
# Ubuntu 24.04 使用 PEP 668，需要使用 --break-system-packages 标志
pip3 install --break-system-packages matplotlib vcdvcd numpy

# 检查 fuzzer 目录是否存在
FUZZER_DIR="/root/mirtl/docker/fuzzer"
if [ ! -d "$FUZZER_DIR" ]; then
    echo "错误: 找不到 $FUZZER_DIR 目录"
    exit 1
fi

echo "复制 fuzzer 到 /fuzzer..."
if [ -d "/fuzzer" ]; then
    rm -rf /fuzzer
fi
cp -r "$FUZZER_DIR" /fuzzer

echo "编译 iverilog-vpi..."
cd /fuzzer
iverilog-vpi dpi_wallclock.c
cd -

# 创建 fuzzer 所需的工作目录
echo "创建 fuzzer 工作目录..."
mkdir -p /scratch/simufuzz-workdir/tmp
chmod -R 777 /scratch/simufuzz-workdir

############################
# 配置 Verismith 环境
############################

echo "=========================================="
echo "配置 Verismith 环境"
echo "=========================================="

# 安装 Nix
echo "安装 Nix..."
mkdir -p /nix
if [ ! -f /etc/profile.d/nix.sh ]; then
    echo "注意: Nix 安装可能需要一些时间..."
    # Nix 安装脚本可能需要以非 root 用户运行，但 --daemon 模式可以在 root 下运行
    curl -L https://nixos.org/nix/install | sh -s -- --daemon || {
        echo "警告: Nix 自动安装可能失败，请手动安装 Nix"
        echo "运行: sh <(curl -L https://nixos.org/nix/install) --daemon"
    }
    # 确保 nix.sh 被加载
    if [ -f /root/.nix-profile/etc/profile.d/nix.sh ]; then
        cp /root/.nix-profile/etc/profile.d/nix.sh /etc/profile.d/nix.sh
        chmod +x /etc/profile.d/nix.sh
    elif [ -f /nix/var/nix/profiles/default/etc/profile.d/nix.sh ]; then
        cp /nix/var/nix/profiles/default/etc/profile.d/nix.sh /etc/profile.d/nix.sh
        chmod +x /etc/profile.d/nix.sh
    fi
fi

# 克隆 Verismith fork
echo "克隆 Verismith fork..."
if [ -d "/verismith" ]; then
    rm -rf /verismith
fi
git clone https://github.com/flaviens/verismith-fork.git /verismith

# 修复依赖问题
echo "修复 Verismith 依赖..."
sed -i 's/tasty-hedgehog >=1.0 && <1.2/tasty-hedgehog >=1.0/g' /verismith/verismith.cabal

# 构建 Verismith
echo "构建 Verismith..."
if [ -f /etc/profile.d/nix.sh ]; then
    source /etc/profile.d/nix.sh
    cd /verismith
    nix-build || {
        echo "警告: nix-build 可能失败，这可能需要一些时间或额外的配置"
        echo "如果失败，请检查 Nix 是否正确安装"
    }
    cd -
else
    echo "警告: 未找到 Nix 环境文件，跳过 Verismith 构建"
    echo "请手动安装 Nix 后运行: cd /verismith && nix-build"
fi

# 复制 eval-verismith
EVAL_VERISMITH_DIR="/root/mirtl/docker/eval-verismith"
if [ ! -d "$EVAL_VERISMITH_DIR" ]; then
    echo "错误: 找不到 $EVAL_VERISMITH_DIR 目录"
    exit 1
fi

echo "复制 eval-verismith 到 /eval-verismith..."
if [ -d "/eval-verismith" ]; then
    rm -rf /eval-verismith
fi
cp -r "$EVAL_VERISMITH_DIR" /eval-verismith

############################
# 验证安装
############################

echo "=========================================="
echo "验证安装"
echo "=========================================="

echo "检查 Yosys..."
if [ -f "/mirtl-yosys/yosys" ]; then
    echo "✓ Yosys 安装成功"
    /mirtl-yosys/yosys --version || true
else
    echo "✗ Yosys 未找到"
fi

echo "检查 Verilator..."
if command -v verilator &> /dev/null; then
    echo "✓ Verilator 安装成功"
    verilator --version || true
else
    echo "✗ Verilator 未找到"
fi

echo "检查 Icarus Verilog..."
if command -v iverilog &> /dev/null; then
    echo "✓ Icarus Verilog 安装成功"
    iverilog -v || true
else
    echo "✗ Icarus Verilog 未找到"
fi

# RUN bash -c "source /etc/profile.d/nix.sh && cd /eval-verismith && python3 eval_performance.py 10 100 /nix/store"
# 运行 Verismith 测试
echo "运行 Verismith 评估..."
source /etc/profile.d/nix.sh
cd /eval-verismith
python3 eval_performance.py 10 100 /nix/store

# 运行 Transfuzz 测试
echo "运行 Transfuzz 测试..."
cd /fuzzer
python3 do_eval_cellthroughput.py 100

python3 do_plot_cellthroughput.py /fuzzer/perfpercell_transfuzz.json /eval-verismith/performance_results.json > /fuzzer/cellthroughput.log
# Cell distributions for Verismith
cd /eval-verismith
python3 gen_manytestcases.py 100 100 /nix/store

# Cell distributions for Transfuzz
cd /fuzzer
python3 do_eval_cellstats.py 100 100

cd /fuzzer
# 🔲 TODO: 下面这行命令会报错，提示No such file or directory: '/eval-verismith/manytestcases/cell_summary_verismith.json'，需要修复
# python3 do_plotcelldistribs.py /fuzzer/eval_cellstats_simufuzz.json /eval-verismith/manytestcases/cell_summary_verismith.json

echo "检查目录结构..."
for dir in /mirtl-yosys /fuzzer /verismith /eval-verismith /nix; do
    if [ -d "$dir" ]; then
        echo "✓ $dir 存在"
    else
        echo "✗ $dir 不存在"
    fi
done

popd

echo "=========================================="
echo "环境配置完成！"
echo "=========================================="
echo ""
echo "已安装的路径："
echo "  - /mirtl-yosys: MiRTL Yosys"
echo "  - /fuzzer: Fuzzer 工具"
echo "  - /verismith: Verismith"
echo "  - /eval-verismith: Verismith 评估工具"
echo "  - /nix: Nix 包管理器"
echo ""
echo "注意: 某些评估脚本可能需要手动运行，请参考 Dockerfile 中的注释"
echo "cd /fuzzer && python3 do_genonebyone.py 4 100 70"
