#include "tcp_server.h"
#include "lwip/tcp.h"
#include <stdint.h>
#include <stdio.h>

#ifdef LWIP_CALLBACK_API
#endif

static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    TCP_Server* server_cfg = (TCP_Server*)arg;  // From tcp_arg

    if (p == NULL) {
        printf("TCP client on port %u disconnected\r\n", server_cfg->port);
        tcp_close(tpcb);
        return ERR_OK;
    }

    if (err != ERR_OK) {
        pbuf_free(p);
        return err;
    }

    // If a receive callback is set, call it
    if (server_cfg && server_cfg->recv_fn) {
        server_cfg->recv_fn(server_cfg, (const uint8_t*)p->payload, p->tot_len);
    }

    // free the received pbuf
    pbuf_free(p);

    return ERR_OK;
}

void tcp_server_send(TCP_Server* cfg, const uint8_t* data, uint16_t len)
{
    if(cfg != NULL && cfg->pcb != NULL)
    {
        err_t err = tcp_write(cfg->pcb, data, len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(cfg->pcb);
        } else {
            printf("TCP server port %u: Error sending data, err=%d\r\n", cfg->port, err);
        }
    }
}

static void tcp_server_err(void *arg, err_t err)
{
    TCP_Server* server_cfg = (TCP_Server*)arg;
    printf("TCP client on port %u error/disconnect, err=%d\r\n", server_cfg->port, err);
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) 
{
    if(arg == NULL) {
        return ERR_VAL;
    }
    
    if (err != ERR_OK || newpcb == NULL)
    {
        return ERR_VAL;
    }

    TCP_Server* server_cfg = (TCP_Server*)arg;
    printf("TCP client connected on port %u\r\n", server_cfg->port);
    server_cfg->pcb = newpcb;

    // Set this config as argument for connection
    tcp_arg(newpcb, server_cfg);

    // Register receive callback
    tcp_recv(newpcb, tcp_server_recv);
    // Register error callback (optional, but good practice)
    tcp_err(newpcb, tcp_server_err);

    return ERR_OK;
}

bool tcp_server_start(TCP_Server *cfg) 
{
    if (cfg->pcb) 
    {
        tcp_server_stop(cfg);
    }
    
    struct tcp_pcb *server_pcb = tcp_new();
    if (!server_pcb) 
    {
        printf("TCP server: Error creating PCB\r\n");
        return false;
    }
    
    if (tcp_bind(server_pcb, IP_ADDR_ANY, cfg->port) != ERR_OK) 
    {
        printf("TCP server: Error binding PCB\r\n");
        tcp_close(server_pcb);
        server_pcb = NULL;
        return false;
    }
    
    struct tcp_pcb *pcb = tcp_listen(server_pcb);
    if (!pcb) 
    {
        printf("TCP server: Error starting to listen\r\n");
        tcp_close(server_pcb);
        server_pcb = NULL;
        return false;
    }

    server_pcb = NULL;
    cfg->pcb = pcb;
    tcp_accept(pcb, tcp_server_accept);
    tcp_arg(pcb, cfg); // // Pass config to accept callback
    printf("TCP server listening on port %d\r\n", cfg->port);
    return true;
}

void tcp_server_stop(TCP_Server *cfg) {
    if (cfg->pcb) {
        tcp_close(cfg->pcb);
        cfg->pcb = NULL;
    }
    printf("TCP server stopped on port %u\n", cfg->port);
}