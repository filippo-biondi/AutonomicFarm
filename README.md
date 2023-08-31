# AutonomicFarm

## Instruction to build and test execution
```bash
git clone https://github.com/filippo-biondi/AutonomicFarm.git
cd AutonomicFarm

# if FastFlow is not installed in your computer
git submodule init
git submodule update

mkdir build
cd build
cmake ..
make

# first test
./test/test_native
./test/test_fastflow

#second test
./test/test_native_latency
./test/test_fastflow_latency

# third test
./test/test_queue
```

## Plotting the result of the test
```bash
cd ..
pip install -r requirements.txt
python3 plot_logs.py test_native.txt test_fastflow.txt test_native_latency.txt test_fastflow_latency.txt test_queue.txt
```
