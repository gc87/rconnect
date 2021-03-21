#ifndef REDIS_CLIENT_H
#define REDIS_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef struct _RedisClient RedisClient;

typedef enum {
    RedisClientError_Success = 0,
} RedisClientError;

typedef int (*SubFunction)(const void *ctx, const unsigned char *data, size_t length);

RedisClientError redis_client_new(RedisClient **client);
RedisClientError redis_client_connect(RedisClient *client);
RedisClientError redis_client_disconnect(RedisClient *client);
RedisClientError redis_client_delete(RedisClient *client);

RedisClientError redis_client_subscribe(RedisClient *client);
RedisClientError redis_client_unsubscribe(RedisClient *client);
RedisClientError redis_client_publish(RedisClient *client);

RedisClientError redis_client_get_key(RedisClient *client);
RedisClientError redis_client_set_key(RedisClient *client);
RedisClientError redis_client_remove_key(RedisClient *client);

RedisClientError redis_client_main_loop(RedisClient *client);

RedisClientError redis_client_next_message(RedisClient *client);

#ifdef __cplusplus
}
#endif

#endif