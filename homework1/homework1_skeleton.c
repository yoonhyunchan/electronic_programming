// 201820873 윤현찬
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 맥북은 window.h를 지원하지 않으므로 unistd.h를 사용
// #include <Windows.h>
#include <unistd.h>

#define ROW 10
#define COL 4

void init_memory(char **memory, int frame_table[][2]);                                 // 메모리를 초기화해주는 함수 (FrameTable도 초기화)
void show_memory(char **memory, int frame_table[][2]);                                 // 메모리의 상태를 보여주는 함수 (FrameTable도 표현)
void memory_allocation(char **memory, char *data, int position, int frame_table[][2]); // 원하는 위치의 메모리에 입력된 값을 할당
void remove_memory(char **memory, int frame_table[][2], int position);                 // 원하는 위치의 메모리를 초기화
void compaction_memory(char **memory, int frame_table[][2]);                           // 메모리 압축

int main()
{

    int position = 0;        // 입력되는 메모리 위치
    int frame_table[ROW][2]; // Frame Table
    char **memory = NULL;
    char data[12] = {
        // 입력 데이터
        "\0",
    };
    char command[12] = ""; // 입력 명령어

    // 메모리 동적 할당
    memory = (char **)malloc(sizeof(char *) * ROW);
    for (int i = 0; i < ROW; i++)
        memory[i] = (char *)malloc(sizeof(char) * (COL + 1));

    init_memory(memory, frame_table); // 메모리 + FramaTable 초기화
    show_memory(memory, frame_table); // 메모리 + FrameTable 출력

    while (1)
    {
        /*

        1. main 함수 구현
        명령어 입력, 데이터 입력, 할당 위치 입력,
        명령에 따른 함수 실행

        */

        // 명령어 입력
        printf("명령 입력 :");
        scanf("%s", command);

        // 입력된 명령어에 따라 다른 동작을 하도록 설정
        // allocation : 입력된 데이터를 원하는 위치에 할당
        // remove : 원하는 위치의 메모리를 초기화
        // compaction : 메모리 압축

        // 1. allocation 명령어를 입력했을 경우
        // strcmp 함수를 통해 입력된 문자열을 확인
        if (strcmp(command, "allocation") == 0)
        {
            // 데어터와 위치 입력
            printf("데이터 입력 : ");
            scanf("%s", data);
            if (strlen(data) <= 12 && strlen(data) > 0)
            {
                printf("size : %lu\n", strlen(data));
                printf("메모리 할당 위치(ROW) 입력 : ");
                scanf("%d", &position);
                // 입력한 위치가 메모리 밖일 때
                if (position <= ROW)
                    memory_allocation(memory, data, position, frame_table);
                else
                    printf("Error Detection\n");
            }
            else
                printf("Error Detection\n");
        }

        // 2. remove 명령어를 입력했을 경우
        else if (strcmp(command, "remove") == 0)
        {
            // 제거할 위치 입력
            printf("제거할 데이터 index : ");
            scanf("%d", &position);

            remove_memory(memory, frame_table, position);
        }

        // 3. compaction 명령어를 입력했을 경우
        else if (strcmp(command, "compaction") == 0)
            compaction_memory(memory, frame_table);

        // +4. exit을 입략하면 while문 종료
        else if (strcmp(command, "exit") == 0)
            break;

        // 3개 이외의 명령어를 입력했을 경우
        else
            printf("Error Detection\n");

        sleep(1);
        system("clear");

        show_memory(memory, frame_table);
    }
    // while문이 끝나면 할당된 메모리 해제
    for (int i = 0; i < ROW; i++)
        free(memory[i]);
    free(memory);

    return 0;
}

// 메모리와 FrameTable을 0으로 초기화한다.
void init_memory(char **memory, int frame_table[][2])
{
    for (int i = 0; i < ROW; i++)
        memset(memory[i], 0, COL);
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < 2; j++)
            frame_table[i][j] = 0;
}

// 현재 메모리 상태(메모리 + FrameTable)를 출력한다.
void show_memory(char **memory, int frame_table[][2])
{

    char value;
    printf("메모리 정보\n\n");

    for (int i = 0; i < ROW; i++)
    {
        printf("%2d ", i);

        for (int j = 0; j < COL; j++)
        {
            value = memory[i][j];
            if (value == 0)
                printf(" □ ");
            else
                printf("%2c ", memory[i][j]);
        }
        printf("  -->  %d %d\n", frame_table[i][0], frame_table[i][1]);
    }
    printf("\n");
}

/*
2. 메모리 할당 함수 구현 (실습 1 참고)
메모리(2차원 배열)의 원하는 위치(position)에 입력된 데이터(data) 삽입
데이터 크기 >= 할당 가능 공간 : 메모리의 해당 위치에 데이터 입력
데이터 크기 <  할당 가능 공간 : 할당 실패

frame_table : 2차원 배열에 데이터 정보(저장 시작 위치, 데이터 크기) 저장
*/
void memory_allocation(char **memory, char *data, int position, int frame_table[][2])
{

    int data_size = strlen(data);            // 입력받은 문자열의 길이
    int require = 1 + (data_size - 1) / COL; // 필요한 메모리의 ROW 개수
    int available = 0;                       // 사용가능한 메모리를 파악하는 데에 쓴다.

    // position(원하는 위치)부터 (원하는 위치 + 필요한 ROW개수 - 1)까지 메모리가 초기화된 상태인지를 확인한다.
    // 추가적으로 메모리의 위치가 ROW보다 크면 안되기 때문에 조건에 추가한다.
    // 원하는 메모리 위치가 비어있는 것을 문자열의 길이를 통해 판단
    // 비어있다면 available을 1씩 증가시켜서 필요한 공간이 비어있는지를 확인한다.
    for (int i = position; (i < position + require) && (i < ROW); i++)
        if (strlen(memory[i]) == 0)
            available++;

    // 앞의 과정에서 필요한 메모리만큼 탐색했을 때 비어있다면
    if (require == available)
    {
        // 입력받은 데이터를 입력한 후 FrameTable도 이에 맞게 수정한다.
        for (int i = 0; i < data_size; i++)
        {
            memory[position + i / COL][i % COL] = data[i];
            // frame_table[position + i / COL][0] = 1;
            // frame_table[position + i / COL][1]++;
            frame_table[position][0] = 1;
            frame_table[position][1] += 1;
        }

        printf("\n할당 완료\n");
        sleep(1);
    }
    // 필요한 메모리가 부족할 경우
    else
    {
        printf("\n할당 실패\n");
        sleep(1);
    }
}

/*
3. 메모리 제거 함수 구현
메모리(2차원 배열)의 원하는 위치(position)에 입력된 데이터(data) 제거
frame_table 확인하여 해당 위치에 데이터 저장되어있을 경우 저장된 데이터 크기만큼 제거
*/

// 원하는 위치의 메모리를 초기화하는 함수
void remove_memory(char **memory, int frame_table[][2], int position)
{
    // FrameTable을 통해 메모리가 비어있는지를 판단하고 비어있다면 오류를 발생시킨다.
    // 0이면 비어있고, 1이면 데이터가 할당되어 있다.
    if (frame_table[position][0] == 0)
    {
        printf("올바르지 않은 인덱스\n");
        return;
    }
    for (int j = 0; j < 1 + (frame_table[position][1] - 1) / 4; j++)
    {
        memset(memory[position + j], 0, sizeof(char) * COL);
    }
    memset(frame_table[position], 0, sizeof(int) * 2);
    // FrameTable을 통해 원하는 위치의 메모리가 차있다면 memset을 통해 메모리와 FrameTable을 0으로 초기화한다.

    printf("제거 성공\n");
}

/*
4. 메모리 압축 함수 구현
frame_table을 확인하여 차례대로 가능한 낮은 ROW의 메모리 위치로 데이터 이동
*/

// 메모리를 압축하기 위한 함수
void compaction_memory(char **memory, int frame_table[][2])
{
    int idx = 0;      // 데이터가 할당되어 있는 위치의 수를 파악하기 위한 변수
    int position = 0; // 메모리의 위치를 저장하는 함수
    char temp[12] = {
        "\0",
    }; // 데이터를 보존하기 위한 임시 문자열 저장 변수 (사용안함)
    int temp_frame_table_C;

    // 0번부터 ROW-1번까지 전체 메모리를 탐색
    for (int i = 0; i < ROW; i++)
    {
        // FrameTable을 통해 메모리가 할당되었는지를 확인한 후
        // 메모리가 할당도었다면 idx를 통해 0번부터 가장 가까운 빈 메모리에 옮긴다.
        if (frame_table[i][0] == 1)
        {
            // trace trap을 방지 (memcpy에서 src와 dest가 같으면 teace trap이 발생한다.)
            temp_frame_table_C = frame_table[i][1];
            if (i == idx)
            {
                idx += 1 + (temp_frame_table_C - 1) / 4;
                continue;
            }

            // 0부터 순차적으로 메모리를 확인 후 메모리가 할당되어 있다면
            // 그 메모리에 있는 값을 idx 위치에 옮긴다.
            // 동작이 끝난 후 다음 메모리 위치로 이동하면 idx는 1 추가된다.

            // strncpy(temp, memory[i], ROW);
            // strncpy(memory[idx], memory[i], ROW);

            for (int j = 0; j < 1 + (temp_frame_table_C - 1) / 4; j++)
            {
                memcpy(memory[idx], memory[i + j], sizeof(char) * ROW);
                memset(memory[i + j], 0, sizeof(char) * COL);
                // strncpy(memory[idx], temp, ROW);

                // FrameTable 복사
                memcpy(frame_table[idx], frame_table[i + j], sizeof(int) * 2);
                memset(frame_table[i + j], 0, sizeof(int) * 2);
                idx++;
                printf("idx: %d\n", idx);
                sleep(1);
            }
        }
    }
    sleep(1);
}