if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <filename>"
  echo "Creates a pair of .h and .cpp files for given name. Files are filled with boilerplate code."
  exit 1
fi

foldername=$(basename `pwd`)
filename=$1
header_file="${filename}.h"
cpp_file="${filename}.cpp"

if [ -e "$header_file" ] || [ -e "$cpp_file" ]; then
  echo "$0: Error, files already exist!"
  exit 1
fi

headerguard="${foldername^^}_${filename^^}_H"

# Create header file
echo -e "#ifndef $headerguard
#define $headerguard

namespace $foldername {


} // namespace $foldername

#endif // $headerguard" > "$header_file"

# Create cpp file
echo -e "#include \"$filename.h\"

namespace $foldername {


} // namespace $foldername" > "$cpp_file"

echo "Files generated: $header_file, $cpp_file"
