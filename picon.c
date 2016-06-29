/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: timg
 *
 * Created on June 17, 2016, 12:47 PM
 */
//#include "cli_shell.h"
#include "simple_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picon.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>

//need to install libconfig-dev package: sudo apt-get install libconfig-dev
//Need to be compiled with flag: -lconfig
#include <libconfig.h>
#include <assert.h>

#define NU_OF_SERIAL_PORTS 16
#define CONFIG_FILE "picon.conf"

//struct termios pgen_opts;
/**
 * handle CTRL+C
 * @param sig signal number
 */
void ctrl_c(int sig)
{
    printf("\r\n\r\nEnter the command picon to return to console\r\n");
    
    //tcsetattr(STDIN_FILENO, TCSANOW, &pgen_opts);
    exit(0);
}
/**
 * Gets port number that ssh session connected to
 * @return integer port number
 */
int get_ssh_port_number();

/**
 * Load from file the mapping of port to device name
 * @param f_name the configuration file name 
 * @param ser_map_in serial_map for return
 * @param num_of_ser number of serial devices
 * @return serial map pointer
 */
int load_serial_map(char *f_name,struct serial_map *ser_map_in,int *num_of_ser);


/*
 * 
 */
int main(int argc, char** argv)
{
    int res;
    //res = tcgetattr(STDIN_FILENO, &pgen_opts);
    assert(res == 0);
    //Catch signals, CTRL+C, CTRL+Z
    signal (SIGINT, ctrl_c);  //ctrl+C
    signal (SIGTSTP, ctrl_c); //ctrl+Z
    struct serial_map serial_m[NU_OF_SERIAL_PORTS];
    int number_of_serial_ports;
    int ret_code = load_serial_map(CONFIG_FILE,&serial_m,&number_of_serial_ports);
    //Debug:
//    int i;
//    for (i=0;i<number_of_serial_ports;i++)
//    printf ("%s,%d\r\n",serial_m[i].device_name,serial_m[i].port_nu);
    if (ret_code==EXIT_FAILURE){
        printf("Failed to load saved file\r\n");
        return(EXIT_FAILURE);
    }
    int port_number,i;
    port_number=get_ssh_port_number();
    if (port_number==22)
    {
        printf("\r\nWelcome!\r\n");
        //cli_shell();
        start_simple_cli();
        exit(EXIT_SUCCESS);
    }
    for (i=0;i<number_of_serial_ports;i++)
    {
        if (serial_m[i].port_nu==port_number)
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                printf("Starting minicom\r\n");
                char external_cmd[200] = "minicom -D ";
                strcat(external_cmd, serial_m[i].device_name);
                system(external_cmd);
            } else 
            {
                waitpid(pid,0,0);
                return (EXIT_FAILURE);
                
            }

            
        }
//            printf("%s\r\n",serial_m[i].device_name);
    }
            
            
            
    return (EXIT_SUCCESS);
}


int get_ssh_port_number()
{
    char return_environment_var[30];
    char *p;
    //	Clear the return_environment_var;
    memset(return_environment_var, "\n", sizeof (return_environment_var));
    //	get the SSH_CLIENT environment var to the return_environment_var
    strcpy(return_environment_var, getenv("SSH_CLIENT"));
    //	extract the port number , that is the number after 2nd space
    p = strtok(return_environment_var, " ");
    p = strtok(NULL, " ");
    p = strtok(NULL, " ");
    //	convert the return_environment_var to integer and return it
    return atoi(p);
}

int load_serial_map(char *f_name,struct serial_map *ser_map_in,int *num_of_ser)
{
    struct serial_map serial_m[NU_OF_SERIAL_PORTS];
    
    
    config_t cfg;
    config_init(&cfg);
    config_setting_t *file_serial_map, *root_conf;
    if (!config_read_file(&cfg, f_name))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return (EXIT_FAILURE);
    }
    file_serial_map = config_lookup(&cfg, "configuration.serial_ports");
    if (file_serial_map != NULL)
    {
        int number_of_ports = config_setting_length(file_serial_map);
        int i;
        for (i = 0; i < number_of_ports; i++)
        {
            config_setting_t *book = config_setting_get_elem(file_serial_map, i);
            const char *dev_name, *port_no;
            int port_nu = 0;
            config_setting_lookup_string(book, "device", &dev_name);
            config_setting_lookup_int(book, "port", &port_nu);
            strcpy(serial_m[i].device_name, dev_name);
            serial_m[i].port_nu = port_nu;
        }
        memcpy(num_of_ser, &i, sizeof (num_of_ser));
        
    }
    memcpy(ser_map_in,serial_m,sizeof(serial_m));
    
    return EXIT_SUCCESS;
}

