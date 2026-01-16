# Ubuntu 24.04 快速开始指南

## 一键安装

```bash
cd /root/mirtl
sudo ./setup-ubuntu24.sh
```

## 安装前准备

1. **下载 mirtl-yosys.tgz**:
   ```bash
   cd /root/mirtl/docker
   mkdir -p mirtl-yosys
   # 从 https://berkeley.box.com/s/lv2jg9fzvwdlimo43i9naks4c1d5u3oh 下载
   # 将文件保存为 docker/mirtl-yosys/mirtl-yosys.tgz
   ```

2. **确认目录存在**:
   - `/root/mirtl/docker/fuzzer/`
   - `/root/mirtl/docker/eval-verismith/`

## 安装后验证

```bash
# 检查工具
/mirtl-yosys/yosys --version
verilator --version
iverilog -v

# 检查目录
ls -la /mirtl-yosys /fuzzer /verismith /eval-verismith
```

## 常用命令

```bash
# 运行 fuzzer 评估
cd /fuzzer
python3 do_eval_execperf.py 100

# 运行 Verismith 评估（需要先加载 Nix 环境）
source /etc/profile.d/nix.sh
cd /eval-verismith
python3 eval_performance.py 10 100 /nix/store
```

## 详细文档

更多信息请参考 `SETUP_UBUNTU24.md`
