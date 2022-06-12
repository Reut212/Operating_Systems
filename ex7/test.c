#include <fcntl.h>
#include "myfs.h"

int main(){
    mymkfs(10000);
//    mymount(NULL,"fs_data",NULL,0,NULL);
    int fd = myopen("home/reut/file",O_CREAT);
//    myclose(fd);
    int fd2 = myopen("home/neta/file", O_CREAT);
    mywrite(fd,"blabla",6);
    mywrite(fd, "bla", 3);
//    mywrite(fd, "bla", 3);
    int index = open_index(fd);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, 4, SEEK_SET);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, 7, SEEK_SET);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, -2, SEEK_CUR); // stuck in an endless loop !!!!
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, 0, SEEK_CUR);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, 3, SEEK_CUR);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, -2, SEEK_END);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd,2,SEEK_CUR);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, 0, SEEK_END);
    printf("block: %d, offset:%d\n", open_f[index].current_block_index, open_f[index].current_offset);
    mylseek(fd, 0, SEEK_SET);
    printf("block: %d, offset:%d\n", open_f[0].current_block_index, open_f[0].current_offset);
    char buf [100];
    myread(fd, buf, 50);
    printf("%s\n",buf);
    mylseek(fd,3,SEEK_SET);
    memset(buf,0,0);
    myread(fd, buf, 50);
    printf("%s\n",buf);
    myclose(fd);
}