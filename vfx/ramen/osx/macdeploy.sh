macdeployqt Ramen.app
cp /usr/local/lib/libtbb.dylib Ramen.app/Contents/Frameworks/
cp /usr/local/lib/libtbbmalloc.dylib Ramen.app/Contents/Frameworks/

install_name_tool -id @executable_path/../Frameworks/libtbb.dylib Ramen.app/Contents/Frameworks/libtbb.dylib
install_name_tool -id @executable_path/../Frameworks/libtbbmalloc.dylib Ramen.app/Contents/Frameworks/libtbbmalloc.dylib

install_name_tool -change /usr/local/lib/libtbb.dylib @executable_path/../Frameworks/libtbb.dylib Ramen.app/Contents/MacOs/Ramen
install_name_tool -change /usr/local/lib/libtbbmalloc.dylib @executable_path/../Frameworks/libtbbmalloc.dylib Ramen.app/Contents/MacOs/Ramen
install_name_tool -change /usr/local/lib/libtbb.dylib @executable_path/../Frameworks/libtbb.dylib Ramen.app/Contents/Frameworks/libtbbmalloc.dylib
