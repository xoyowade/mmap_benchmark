# don't repet to avoid cache

bind=0

if [ $# -eq 1 ]; then
bind=$1
fi

make > /dev/null || exit 1
format=csv
#format=tabular
testnum=11

mkdir -p data
for i in  $(seq -f "%02g" 1 $testnum); do
sudo umount data
sudo mount -t tmpfs -o size=4G -o mpol=bind:$bind none data
taskset -c 8 ./mmap --benchmark_min_time=0.01 --benchmark_repetitions=1 --benchmark_filter=BM_MMap$i --benchmark_format=$format --benchmark_report_aggregates_only=true
done

sudo umount data

