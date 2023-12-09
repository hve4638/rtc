# RTC Command

## Usage

아무 인자도 지정하지 않았다면 시스템 시간과 RTC 시간을 표시합니다

```
Argument:
    -h  help 표시
    -l  RTC 시간으로 시스템 시간을 초기화
    -s  시스템 시간으로 RTC 시간을 초기화
```


## 코드의 구조

```
-main.c
--rtc.c
--rtc-sysfs.c
--systime.c
--printbuffer.c
```

`main.c`
- 프로그램 진입점 main() 포함됨
- 인자 처리 및 


`rtc.c`
- ioctl 인터페이스를 이용한 rtc 읽기/쓰기 구현

`rtc-sysfs.c`
- sysfs를 이용한 rtc 읽기 구현
- sysfs을 사용하지 않는 경우

`systime.c`
- 시스템 시간 읽기/쓰기 구현

`printbuffer.c`
- 디버그 전용

## 코드 상세

### main.c

#### *의존성*

- `rtc.h`
- `systime.h`
- `rtc.c`
- `systime.c`

#### *main.c에서 수행하는 것*

- 인자 처리
- rtc 시간 및 시스템 시간 표시
- rtc 시간으로 시스템 시간 동기화, 또는 반대로 동기화

#### *인자 처리*

```c
#define FLAG_ARGERROR   0x1 // 예상치 못한 옵션 처리
#define FLAG_HELP       0x2 // -h 옵션 처리
#define FLAG_LOAD       0x4 // -l 옵션 처리
#define FLAG_STORE      0x8 // -s 옵션 처리
#define FLAG_TEST      0x10 // 디버깅 전용

int catch_args(int argc, char *const* argv);
```

main() 에서 받은 인자를 넘겨줘 옵션을 처리합니다

각 인자를 처리후 해당하는 4바이트 비트플래그를 리턴합니다

비트 플래그는 메크로를 참조하세요

#### *인자에 따른 작업 분기*

```c
int main(int argc, char *const* argv) {
    int flags = catch_args(argc, argv);

    if (flags & FLAG_ARGERROR) {
        exit(-1);
    }
    else if (flags & FLAG_HELP) {
        show_help(); // help 출력
    }
    else if (flags & FLAG_LOAD) {
        load_systime_from_rtc(); // systime->rtctime 동기화
    }
    else if (flags & FLAG_STORE) {
        store_systime_to_rtc(); // rtctime->systime 동기화
    }
    else {
        show_clock(); // systime, rtctime 표시
    }

    return 0;
}
```

`catch_args`에서 받은 플래그로 분기를 처리합니다

### rtc.c

#### *의존*

X

#### *rtc.c에서 수행하는 것*

- `/dev/rtcN` 장치 찾아 열기
- `ioctl`을 통한 `rtc` 시간 반환 및 설정
- `rtc` 알람 인터럽트 등 `ioctl`에서 제공하는 요청 처리

#### *rtc.h*

```c
void rtc_open();
void rtc_open_rw();
void rtc_open_ro();
void rtc_close();

// ioctl에 정의된 요청 처리
void rtc_readtime(struct tm*);
void rtc_writetime(struct tm*);
unsigned long rtc_readepoch();
void rtc_uie_on();
void rtc_uie_off();
void rtc_alarm_set(struct tm *time);
void rtc_alarm_read(struct tm *time);
void rtc_alarm_on();
void rtc_alarm_off();
void rtc_wkalarm_set(struct rtc_wkalrm *);
void rtc_wkalarm_read(struct rtc_wkalrm *);
```

- `ioctl`에서 제공하는 요청이라도 모든 RTC장치가 해당 요청을 수행할 수 있는 것은 아니다
- `rtc`시간을 읽고 쓰는 요청은 모든 RTC에서 처리할 수 있다 ()
