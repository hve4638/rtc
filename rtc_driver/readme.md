# RTC Driver 재구현

## 빌드 예시

```bash
make -C ~/embedded-linux-beagleplay-labs/kernel/linux M=$PWD ARCH=arm64 CROSS_COMPILE=aarch64-linux-

sudo make -C ~/embedded-linux-beagleplay-labs/kernel/linux M=$PWD ARCH=arm64 CROSS_COMPILE=aarch64-linux- INSTALL_MOD_PATH=$HOME/embedded-linux-beagleplay-labs/tinysystem/nfsroot modules_install
```

- `~/embedded-linux-beagleplay-labs/kernel/linux` : linux 커널 위치입니다
- `~/embedded-linux-beagleplay-labs/tinysystem/nfsroot`  타겟 시스템의 루트디렉토리를 나타냅니다


## 상세

*PPT 참조*