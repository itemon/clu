
# build the deps for xcode

cwd=`pwd`
bin_dir=${BUD_DIR}/../../bin/

echo "build dylib for xcode in dir ${bin_dir}"

# make xcode dir
mkdir -p ${bin_dir}/xcode

cp -f ${bin_dir}/*.dylib ${bin_dir}/xcode/
cp -f ${bin_dir}../../src/mytest/*.h ${bin_dir}/xcode/

all_dylibs=`find ${bin_dir}/xcode -type f -name *.dylib`
for lib in $all_dylibs
do
  name=`basename $lib`
  if [ -L $lib ]; then
    echo "$lib->@rpath/$name"
  else
    install_name_tool -id "@rpath/$name" $lib
  fi
done