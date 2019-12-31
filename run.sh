input="$1"

./9cc "$input" > tmp.s
gcc -o tmp tmp.s
./tmp
echo $?
