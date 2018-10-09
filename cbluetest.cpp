#include <sys/ioctl.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

using namespace std;

//#define HCI_MAX_DEV 64

#define BLUE_ERROR 2

#define BLUE_NOT_CONNECTED -1

int error = 0;

int GetRSSI(const char *address)
{

    struct hci_conn_info_req *cr;
    int8_t rssi;
    int dd;
    int dev_id =0;
    
    error = false;

    struct sockaddr_l2 addr = { 0 };
    int sock, status;

    dd = hci_open_dev(dev_id);
    if (dd < 0)
    {
        perror("Could not open HCI device");
        return BLUE_ERROR;
    }

    if ((cr = (hci_conn_info_req *)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info))) == NULL)
    {
        perror("malloc");
        hci_close_dev(dd);
        return BLUE_ERROR;
    }

    sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    addr.l2_family = AF_BLUETOOTH;
    //service discovery port
    addr.l2_psm = htobs(0x1);
    str2ba( address, &addr.l2_bdaddr );

    // connect to target device
    status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if(status!=0)
    {
        perror("Could not connect to dev");
        free(cr);
        hci_close_dev(dd);
        close(sock);
        return BLUE_ERROR;
    }

    bacpy(&cr->bdaddr, &addr.l2_bdaddr);
    cr->type = ACL_LINK;

    if (ioctl(dd, HCIGETCONNINFO, (unsigned long)cr) < 0)
    {
        perror("Could not get connection info");
        free(cr);
        hci_close_dev(dd);
        close(sock);
        return BLUE_ERROR;
    }

    int count = 30;
    while( count>0 )
    {
        if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &rssi, 0) < 0)
        {
            perror("Could not read RSSI");
            free(cr);
            hci_close_dev(dd);
            close(sock);
            return BLUE_ERROR;
        }
        cout <<"rssi: " << int(rssi) << endl;
        sleep(1);
        count--;
    }

    free(cr);
    hci_close_dev(dd);
    close(sock);
    return rssi;
}

int main(int argc, char **argv)
{
    while(true)
        GetRSSI("88:19:08:2C:EA:C2");
}