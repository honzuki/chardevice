#define SIZE 512
struct cd_dev
{
        char buf[SIZE];
        struct cdev cdev;
};
