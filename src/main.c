#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <getopt.h>
#include <elf.h>
#include <err.h>
#include <string.h>

#include "main.h"
#include "elf-multiarch.h"
#include "elf-common.h"
#include "common.h"
#include "io.h"

void parser_opts(int argc, char **argv, struct scdump_options *cmd){
    char *ptr;
    int opt;
    static struct option loptions[] = {
        {"section", required_argument, 0, 'S'},
        {"symbol", required_argument, 0, 's'},
        {"address", required_argument, 0, 'a'},
        {"output", required_argument, 0, 'o'},
        {"raw", no_argument, 0, 'r'},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {NULL, 0, NULL, 0}
    };

    memset(cmd, 0x0, sizeof(struct scdump_options));
    cmd->options.fd_out = 1;

    while((opt = getopt_long(argc, argv, "S:s:a:o:rhv", loptions, NULL)) != -1){
        switch(opt){
            case 'S':
                cmd->options.section = optarg;
            break;
            case 's':
                cmd->options.symbol = optarg;
            break;
            case 'a':
                cmd->options.addr.addr_64 = strtol(optarg, &ptr, 16);
                if(*ptr++ != ',')
                    errx(1, "-a, --adress: invalid paramater");
                cmd->options.size.addr_64 = strtol(ptr, NULL, 10);
            break;
            case 'o':
                cmd->output = optarg;
            break;
            case 'r':
                cmd->options.raw = 1;
            break;
            case 'v':
                printf("%s\n", VERSION);
                exit(0);
            break;
            case 'h':
                cmd->help = 1;
            break;
            default:
                exit(1);
        }
    }

    if(optind < argc)
        cmd->filename = argv[optind];
}


void help(void){
    static const char msg[]=
        "-- scdump v" VERSION " --\n"
        "Usage: scdump [file] [options]\n\n"
        "Options:\n"
        "  -S, --section <name>   Dump an entire section\n"
        "  -s, --symbol <name>    Dump shellcode by symbol name\n"
        "  -a, --addr <hexnumber,size>\n"
        "                         Dump shellcode based on virtual address\n"
        "  -o, --output <file>    Write to file instead of stdout\n"
        "  -r, --raw              Raw output\n"
        "  -v, --version          print current version\n"
        "  -h, --help             Display this help menu";

    puts(msg);
    exit(0);
}

int main(int argc, char **argv){
    struct scdump_options cmd;
    union elfHeader header;

    parser_opts(argc, argv, &cmd);
    if(cmd.help)
        help();

    if(!cmd.filename || (!cmd.options.section && !cmd.options.symbol && !cmd.options.size.addr_64)){
        die("try 'scdump --help' for further information\n");
    }

    rdonly(cmd.filename, &(cmd.options.fh));

    if(cmd.output){
        cmd.options.fd_out = open(cmd.output, O_WRONLY|O_CREAT, 0644);
        if(cmd.options.fd_out == -1)
            err(1, "open");
    }

    switch(get_elf_arch(cmd.options.fh.fd, &header, sizeof(header))){
        case ELFCLASS32:
            extract_shellcode32(&header.header_32, &cmd.options);
        break;
        case ELFCLASS64:
            extract_shellcode64(&header.header_64, &cmd.options);
        break;
        case -1:
            err(1, "read");
        break;
        default:
            warn("invalid elf ...\n");
            return 1;
    }

    return 0;
}
