
bin_dir=/Users/huangwei/code/prj/serve/clucene/build-name/bin
xcode=$bin_dir/xcode
version=2.3.3.4
revision=1

mkdir -p $xcode

# for shared
lib_shared=libclucene-shared.$revision.dylib
lib_shared_no_version=libclucene-shared.dylib

cp -f $bin_dir/libclucene-shared.${version}.dylib $xcode/$lib_shared_no_version
sudo install_name_tool -change ${bin_dir}/${lib_shared} "@rpath/${lib_shared_no_version}" $xcode/$lib_shared_no_version
sudo install_name_tool -id "@rpath/${lib_shared_no_version}" $xcode/$lib_shared_no_version

echo "\n------------for shared---------------\n"
otool -L $bin_dir/$lib_shared
otool -L $xcode/$lib_shared_no_version

# for core
lib_core=libclucene-core.$revision.dylib
lib_core_no_version=libclucene-core.dylib
cp -f $bin_dir/libclucene-core.${version}.dylib $xcode/$lib_core_no_version
sudo install_name_tool -id "@rpath/${lib_core_no_version}" $xcode/$lib_core_no_version
sudo install_name_tool -change ${bin_dir}/${lib_shared} "@rpath/${lib_shared_no_version}" $xcode/$lib_core_no_version

echo "\n------------for core---------------\n"
otool -L $bin_dir/$lib_core
otool -L $xcode/$lib_core_no_version

#for contrib
lib_contri=libclucene-contribs-lib.$revision.dylib
lib_contri_no_version=libclucene-contribs-lib.dylib
cp -f $bin_dir/libclucene-contribs-lib.${version}.dylib $xcode/$lib_contri_no_version
sudo install_name_tool -id "@rpath/${lib_contri_no_version}" $xcode/$lib_contri_no_version
sudo install_name_tool -change ${bin_dir}/${lib_shared} "@rpath/${lib_shared_no_version}" $xcode/$lib_contri_no_version
sudo install_name_tool -change ${bin_dir}/${lib_core} "@rpath/${lib_core_no_version}" $xcode/$lib_contri_no_version

echo "\n------------for contri---------------\n"
otool -L $bin_dir/$lib_contri
otool -L $xcode/$lib_contri_no_version

#for my_test
lib_mytest=libmy_test.dylib
cp -f $bin_dir/$lib_mytest $xcode/$lib_mytest
sudo install_name_tool -id "@rpath/${lib_mytest}" $xcode/$lib_mytest
sudo install_name_tool -change ${bin_dir}/${lib_shared} "@rpath/${lib_shared_no_version}" $xcode/$lib_mytest
sudo install_name_tool -change ${bin_dir}/${lib_core} "@rpath/${lib_core_no_version}" $xcode/$lib_mytest
sudo install_name_tool -change ${bin_dir}/${lib_contri} "@rpath/${lib_contri_no_version}" $xcode/$lib_mytest

echo "\n------------for my_test---------------\n"
otool -L $bin_dir/$lib_mytest
otool -L $xcode/$lib_mytest

cp -f src/mytest/*.h $xcode/
echo "all done"
