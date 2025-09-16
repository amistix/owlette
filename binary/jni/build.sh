if [ -z "$ANDROID_NDK_HOME" -a "$ANDROID_NDK_HOME" == "" ]; then
	echo -e "\033[31mFailed! ANDROID_NDK_HOME is empty'\033[0m"
	exit 1
fi

export PATH=$ANDROID_NDK_HOME:$PATH
export NDK_MODULE_PATH=$(pwd)
echo $NDK_MODULE_PATH

echo "Building boost..."
sh build_boost.sh 

echo "Building openssl..."
sh build_openssl.sh

echo "Building miniupnpc..."
sh build_miniupnp.sh

echo "Cleaning..."
$ANDROID_NDK_HOME/ndk-build clean

echo "Building i2pd"
$ANDROID_NDK_HOME/ndk-build 
