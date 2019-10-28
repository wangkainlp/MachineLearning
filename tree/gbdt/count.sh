
# awk -F ':' 'BEGIN{sum=0;cnt=0;}{cnt++;sum += $2}END{print sum, sum/cnt}'
awk -F ':' 'BEGIN{sum=0;cnt=0;max=-1;min=100;}{cnt++;sum += $1; if($1>max){max=$1;} if($1<min){min=$1}}END{print sum, sum/cnt, max, min, cnt}'
