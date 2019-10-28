

for ((i=1; i<=12; ++i)); do
	cnt=$(echo "12*$i" | bc)
	# echo $cnt 
	cat log | grep func_ | head -n $cnt  | tail -n 12 | cut -f 2 -d ':' | sh count.sh
done
