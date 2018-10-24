#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define HASH_SIZE 100
#define URL_LENGTH 1000
#define SEED_URL "https://www.chitkara.edu.in/"
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
void insert(char *myurl, struct HASH h[], int d, char *str[], int size)
{
    struct LL *start;
    int i = 0, k, j, flag = 0;
    while (i < size)
    {
        // if (i != size - 1)
        //     printf("-->%s\n==>%s\n\n", str[i], str[i + 1]);
        struct LL *n = (struct LL *)malloc(sizeof(struct LL));
        CHECK_MALLOC(n);
        n->next = NULL;
        if (strcmp(myurl, str[i]) == 0)
        {
            n->isvistited = 1;
            n->u.depth = d;
        }
        else
        {
            n->isvistited = 0;
            n->u.depth = d + 1;
        }
        k = strlen(str[i]) % 100;
        if (h[k].head == NULL)
        {
            n->u.str = str[i];
            n->u.key = k;
            h[k].head = n;
            h[k].end = n;
        }
        else
        {
            flag = 0;
            start = h[k].head;
            while (start != h[k].end)
            {
                if (strcmp(start->u.str, str[i]) == 0)
                {
                    flag = 1;
                    break;
                }
                else
                    start = start->next;
            }
            if (strcmp(h[k].end->u.str, str[i]) == 0)
            {
                flag = 1;
            }
            if (flag != 1)
            {
                n->u.str = str[i];
                n->u.key = k;
                h[k].end->next = n;
                h[k].end = n;
            }
        }

        i++;
    }
    i = 0;
    j = 0;
    while (j < 99)
    {
        while (h[i].head == NULL)
        {
            i++;
        }
        j = i + 1;
        while (h[j].head == NULL)
        {
            j++;
        }
        h[i].end->next = h[j].head;
        //printf("%d %d\n", i, j);
        i = j;
    }
    h[j].end->next = NULL;
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
void check_myurl(char *url)
{
    char str[URL_LENGTH + 100] = "wget --spider ";
    strcat(str, url);
    //printf("%s",str);
    if (!system(str))
        printf("Valid URL");
    else
        printf("Invalid URL");
}
void check_valid(char *dir, int depth)
{
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
void make_dir(char *p)
{
    char f[] = ".txt", itostring[5];
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
    // while (i != strlen(url))
    // {
    //     if (url[i] == '.')
    //     {
    //         char ext[4];
    //         ext[0] = url[i];
    //         ext[1] = url[i + 1];
    //         ext[2] = url[i + 2];
    //         ext[3] = url[i + 3];
    //         //printf("\n\n%s---%s\n\n", url, ext);
    //         if (strcmp(ext, ".css") == 0 || strcmp(ext, ".php") == 0 || strcmp(ext, ".ico") == 0 || strcmp(ext, ".jpg") == 0 || strcmp(ext, ".zip") == 0 || strcmp(ext, ".pdf") == 0 || strcmp(ext, ".xml") == 0 || strcmp(ext, ".xls") == 0 || strcmp(ext, ".rar") == 0)
    //         {
    //             return 4;
    //         }
    //     }
    //     i++;
    // }
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
int NormalizeURL(char *URL)
{
    int len = strlen(URL);
    if (len <= 1)
        return 0;
    //! Normalize all URLs.
    if (URL[len - 1] == '/')
    {
        URL[len - 1] = 0;
        len--;
    }
    int i, j;
    len = strlen(URL);
    //! Safe check.
    if (len < 2)
        return 0;
    //! Locate the URL's suffix.
    for (i = len - 1; i >= 0; i--)
        if (URL[i] == '.')
            break;
    for (j = len - 1; j >= 0; j--)
        if (URL[j] == '/')
            break;
    //! We ignore other file types.
    //! So if a URL link is to a file that are not in the file type of the following
    //! one of four, then we will discard this URL, and it will not be in the URL list.
    if ((j >= 7) && (i > j) && ((i + 2) < len))
    {
        if ((!strncmp((URL + i), ".htm", 4)) || (!strncmp((URL + i), ".HTM", 4)))
        {
            len = len; // do nothing.
        }
        else
        {
            return 0; // bad type
        }
    }
    return 1;
}
void printLL(struct LL *start)
{
    int i = 0;
    while (start != NULL)
    {
        printf("%d d=%d v=%d %d %s\n", i++, start->u.depth, start->isvistited, start->u.key, start->u.str);
        start = start->next;
    }
}
void work(char *myurl, struct HASH *h, char *dir, int depth)
{
    char *d = (char *)malloc(sizeof(char) * 2000), *str;
    strcpy(d, dir);
    strcat(d, "/temp.txt");
    struct LL *start = h[0].head;
    char *url = (char *)malloc(sizeof(char) * (URL_LENGTH + 1));
    char *url_list[MAX_URL_PER_PAGE] = {0};
    int i, j = 0, cur_depth = 0, flag = 0;
    i = 0;
    url_list[i++] = SEED_URL;
    for (j = 0;; j++)
    {

        if (j == 0)
        {
            i = 1;
        }
        else
        {
            i = 0;
        }
        memset(d, 0, sizeof(d));
        strcpy(d, dir);
        strcat(d, "/temp.txt");
        getpage(myurl, d);
        str = get_in_string(d);
        memset(d, 0, sizeof(d));
        strcpy(d, dir);
        make_dir(d);
        printf("%s\n", d);
        write_to_file(d, str);
        while (i < MAX_URL_PER_PAGE && url_extract(url, URL_LENGTH, &str))
        {
            //printf("%s", url);
            if (check_url(url, url_list, i) == 0 && NormalizeURL(url) == 1)
            {
                char *t = (char *)malloc(sizeof(char) * (URL_LENGTH + 1));
                CHECK_MALLOC(t);
                strcpy(t, url);
                url_list[i] = t;
                i++;
            }
        }
        memset(str, 0, strlen(str));
        insert(myurl, h, cur_depth, url_list, i);
        //printLL(h[0].head);
        start = h[0].head;
        while (start->isvistited == 1 || start->u.depth > depth)
        {
            //printf("-->%d*****%s\n", cur_depth, start->u.str);
            start = start->next;
            if (start == NULL)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 1)
        {
            break;
        }
        start->isvistited = 1;
        myurl = start->u.str;
        check_myurl(myurl);
        cur_depth = start->u.depth;
    }
    printLL(h[0].head);
    system("clear");
    printf("\n\n\t\tJOB DONE\n\n");
}
int main()
{
    struct HASH *h = (struct HASH *)malloc(sizeof(struct HASH) * 101);
    int depth;
    char *dir = (char *)malloc(sizeof(char) * 2000);
    CHECK_MALLOC(dir);
    printf("enter directory and depth\n");
    scanf("%s%d", dir, &depth);
    check_valid(dir, depth);
    work(SEED_URL, h, dir, depth);
    return 0;
}
