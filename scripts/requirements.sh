for dependency in $(ldd $1 | awk '{print $3}')
do
    dlocate $dependency | awk -F '[: ]' '{print $1}' | head -n 1 >> requirements.txt
done
sort requirements.txt | uniq | tee requirements.txt >> /dev/null
