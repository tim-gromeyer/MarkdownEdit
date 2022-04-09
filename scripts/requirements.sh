usage()
{
    echo "Usage:"
    echo -e "\t$0 <path-to-executable> [options]"
    echo ""
    echo "Options:"
    echo -e "\t-h\tShow this message and exit."
    echo -e "\t-o\tSet the output file."
    exit 1
}

while getopts "o:h" flag
do
    case "${flag}" in
        o) out=${OPTARG};;
        h) usage;;
        ?) usage;;
    esac
done

if [ -z "$1" ]; then
    usage
fi

if [ ! -f "$1" ]; then
    echo "$1 does not exist."
    exit 1
fi

if [ -z "$out" ]; then
    out="requirements.txt"
fi

for dependency in $(ldd $1 | awk '{print $3}')
do
    dlocate $dependency | awk -F '[: ]' '{print $1}' | head -n 1 >> "$out"
done
sort "$out" | uniq | tee "$out" >> /dev/null
