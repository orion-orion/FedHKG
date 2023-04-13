release_dir=release
if [ -d "$release_dir" ]; then
  rm -rf $release_dir
  mkdir $release_dir
fi
if [ $1 = "x86" ]; then
    g++ ./base/Base.cpp -fPIC -shared -o ./release/Base.so -pthread -O3
elif [ $1 = "apple-m1" ]; then
    g++ ./base/Base.cpp -fPIC -shared -o ./release/Base.so -pthread -O3 -mcpu=apple-m1
else
    echo "invalid computer architecture"
fi