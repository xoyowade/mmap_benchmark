function collect {
  fn=$1
  echo "name,iterations,real_time,cpu_time,time_unit,bytes_per_second,items_per_second,label,error_occurred,error_message" > $fn 
  grep "BM" rawlog >> $fn
}

rm -f rawlog

for i in $(seq 1 10); do
echo "=== Iteration $i ==="
./run.sh 0 >> rawlog
done

collect res_same.csv
python sum.py res_same.csv

rm -f rawlog

for i in $(seq 1 10); do
echo "=== Iteration $i ==="
./run.sh 1 >> rawlog
done

collect res_diff.csv
python sum.py res_diff.csv
