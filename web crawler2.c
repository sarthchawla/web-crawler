#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define HASH_SIZE 100
#define URL_LENGTH 1000
#define SEED_URL "https://www.chitkara.edu.in"
#define MAX_URL_PER_PAGE 1000
#define INTERVAL_TIME 10
int file_no = 1; //global decalartion of file number
#define CHECK_MALLOC(s)                                                       \
    if ((s) == NULL)                                                          \
    {                                                                         \
        printf("memory allocation failed in %s: line%d", __FILE__, __LINE__); \
        perror(":");                                                          \
        exit(-1);                                                             \
    }
#define SET_MEM(start, size) memset(start, 0, size)
struct URL
{
    int depth, key;
    char *str;
};
struct LL
{
    int isvistited;
    struct LL *next;
    struct URL u;
};
struct HASH
{
    struct LL *head, *end, *last_visited;
};
void insert(struct HASH h[], int d, char *str[], int size)
{
    struct LL *start;
    int i = 0, k;
    while (i < size)
    {
        struct LL *n = (struct LL *)malloc(sizeof(struct LL));
        CHECK_MALLOC(n);
        n->next = NULL;
        n->isvistited = 0;
        n->u.depth = d;
        k=strlen(str[i])%10;
        if (h[k].head == NULL)
        {
            n->u.str = str[i];
            n->u.key = k;
            h[k].head = n;
            h[k].end = n;
        }
        else
        {
            start=h[k].head;
            while(start!=h[k].end)
            {
                if(strcmp(start->u.str,str[i])==0)
                {
                    i++;
                    k=strlen(str[i])%10;
                    start=h[k].head;
                }
                start=start->next;
            }
            n->u.str = str[i];
            n->u.key = k;
            h[k].end->next = n;
            h[k].end = n;
        }

        //printf("%s\n", h[k].end->u.str);
        i++;
    }
    int j;
    for (i = 0; i < 9; i++)
    {
        j = 1;
        while (h[i + j].head == NULL)
        {
            j++;
        }
        h[i].end->next = h[i + j].head;
    }
    //return start;
}
char *tag = "href";
char to_lower(char c)
{
    return c > 92 ? c : c + 32;
}

char is_whitespace(char c)
{
    return c == '\t' || c == '\n' || c == ' ';
}

int url_extract(char *str, int n, char **p)
{
    char *t = *p, *c = tag;
    char quote;

    while (*t != '\0')
    {
        while (*t && *c && to_lower(*t) == *c)
        {
            t++;
            c++;
        }

        if (*c == '\0')
        {
            int i = 0;
            while (*t != '\0' && is_whitespace(*t))
                t++;
            if (*t++ == '=')
            {
                while (*t != '\0' && is_whitespace(*t))
                    t++;
                if (*t == '\'' || *t == '\"')
                {
                    quote = *t++;
                    while (*t != '\0' && *t != quote && i < n - 1)
                        str[i++] = *t++;
                    str[i] = '\0';
                }
            }
            if (i != 0)
                break;
        }
        c = tag;
        t++;
    }

    *p = t;
    return *t;
}
void getpage(char *url, char *dir)
{
    char *urlbuffer = (char *)malloc(sizeof(char) * (URL_LENGTH + 300));
    strcat(urlbuffer, "wget ");
    strcat(urlbuffer, url);
    strcat(urlbuffer, " -O ");
    strcat(urlbuffer, dir);
    //strcat(urlbuffer, "/temp.txt");
    //strcat(urlbuffer, " --proxy-user=user1 --proxy-password=user1");
    system(urlbuffer);
    free(urlbuffer);
}
void testDir(char *dir)
{
    struct stat statbuf;
    if (stat(dir, &statbuf) == -1)
    {
        fprintf(stderr, "-----------------\n");
        fprintf(stderr, "Invalid directory\n");
        fprintf(stderr, "-----------------\n");
        exit(1);
    }

    //Both check if there's a directory and if it's writable
    if (!S_ISDIR(statbuf.st_mode))
    {
        fprintf(stderr, "-----------------------------------------------------\n");
        fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
        fprintf(stderr, "-----------------------------------------------------\n");
        exit(1);
    }

    if ((statbuf.st_mode & S_IWUSR) != S_IWUSR)
    {
        fprintf(stderr, "------------------------------------------\n");
        fprintf(stderr, "Invalid directory entry. It isn't writable\n");
        fprintf(stderr, "------------------------------------------\n");
        exit(1);
    }
}
void check_valid(char *dir, int depth)
{
    int i, j;
    if (!system("wget --spider https://chitkara.edu.in"))
        printf("Valid URL");
    else
        printf("Invalid URL");

    //printf("*%s*",d);
    testDir(dir);
    if (depth < 0)
    {
        printf("depth entered is negative\n");
        perror(":");
        exit(-1);
    }
}
char *get_in_string(char *dir)
{
    struct stat statbuf;
    stat(dir, &statbuf);
    long int n = (statbuf.st_size) + 2;
    char *buff = (char *)malloc(sizeof(char) * n);
    CHECK_MALLOC(buff);
    long int i = 0;
    FILE *fptr;
    char c;
    fptr = fopen(dir, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
    c = fgetc(fptr);
    while (c != EOF)
    {
        //printf("\n%c", c);
        buff[i] = c;
        i++;
        c = fgetc(fptr);
    }
    strcat(buff, "\0");
    fclose(fptr);
    return buff;
}
void *make_dir(char *p)
{
    char f[] = ".txt", itostring[2];
    sprintf(itostring, "%d", file_no++); //converts integer into string
    strcat(p, "/");
    strcat(p, itostring);
    strcat(p, f);
    printf("\nPATH has been created\n");
}
void write_to_file(char *d, char *str)
{
    FILE *f;
    f = fopen(d, "w");
    fprintf(f, "%s", str);
    //printf("%s", str);
    fclose(f);
}
int check_url(char *url, char *url_list[], int n)
{
    int i;
    if (strlen(url) <= strlen(SEED_URL))
    {
        return 1;
    }
    else
    {
        for (i = 0; SEED_URL[i] != '\0'; i++)
        {
            if (url[i] != SEED_URL[i])
            {
                return 2;
            }
        }
    }
    i = 0;
    while (i < n)
    {
        if (strcmp(url, url_list[i]) == 0)
        {
            //printf("**%d\t%s**\n",i,url_list[i]);
            return 3;
        }
        i++;
    }
    return 0;
}
void printLL(struct LL *start)
{
    while (start != NULL)
    {
        printf("%d %s\n", start->u.key, start->u.str);
        start = start->next;
    }
}
void work(char *myurl, struct HASH *h, char *dir, int depth)
{
    struct LL *start = NULL;
    //static int mode = 0;
    int i, j;
    char *d = (char *)malloc(sizeof(char) * 2000);
    strcpy(d, dir);
    strcat(d, "/temp.txt");
    getpage(myurl, d);
    //printf("%s\n", d);
    char *str = get_in_string(d);
    strcpy(d, dir);
    make_dir(d);
    printf("%s\n", d);
    write_to_file(d, str);
    char *url = (char *)malloc(sizeof(char) * (URL_LENGTH + 1));
    char *url_list[MAX_URL_PER_PAGE] = {0};
    i = 0;
    url_list[i++] = SEED_URL;
    while (i < MAX_URL_PER_PAGE && url_extract(url, URL_LENGTH, &str))
    {

        if (check_url(url, url_list, i) == 0)
        {
            char *t = (char *)malloc(sizeof(char) * (URL_LENGTH + 1));
            strcpy(t, url);
            url_list[i] = t;
            //printf("%s %d\n", url_list[i], i);
            i++;
        }
    }
    insert(h, depth, url_list, i);
    printLL(h[0].head);
}
int main(int *argc, char *argv[])
{
    struct HASH *h = (struct HASH *)malloc(sizeof(struct HASH) * 10);
    int depth, i = 0;
    char *dir = (char *)malloc(sizeof(char) * 2000);
    CHECK_MALLOC(dir);
    printf("enter directory and depth\n");
    scanf("%s%d", dir, &depth);
    check_valid(dir, depth);
    work(SEED_URL, h, dir, depth);
    return 0;
}