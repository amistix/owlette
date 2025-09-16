if [ -z "$ANDROID_NDK_HOME" -a "$ANDROID_NDK_HOME" == "" ]; then
	echo -e "\033[31mFailed! ANDROID_NDK_HOME is empty'\033[0m"
	exit 1
fi

echo "Building boost..."
sh build_boost.sh 

echo "Building miniupnpc..."
sh build_miniupnp.sh
