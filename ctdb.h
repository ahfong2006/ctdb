/*
 * 
 * Copyright (c) 2021, Joel
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __CTDB_H_
#define __CTDB_H_
#ifdef __cplusplus
extern "C" {
#endif

#define CTDB_CHAR_LEN 1
#define CTDB_I32_LEN 4
#define CTDB_I64_LEN 8

#define CTDB_HEADER_SIZE 128
#define CTDB_MAGIC_STR "ctdb"
#define CTDB_MAGIC_LEN 4
#define CTDB_VERSION_NUM 1

#define CTDB_MAX_KEY_LEN 64
#define CTDB_MAX_CHAR_RANGE 256

#define CTDB_MAX_VALUE_LEN (1024 * 1024 * 1024) //1G

#define CTDB_ITEMS_SIZE (CTDB_CHAR_LEN + CTDB_I64_LEN) //sub_prefix_char, sub_node_pos
#define CTDB_NODE_SIZE (CTDB_CHAR_LEN + CTDB_MAX_KEY_LEN + CTDB_I64_LEN + CTDB_CHAR_LEN) //prefix_len, prefix, leaf_pos, items_count

#define CTDB_LEAF_SIZE (CTDB_I32_LEN + CTDB_I64_LEN) //value_len, value_pos

#define CTDB_FOOTER_ALIGNED_BASE (32)
#define CTDB_FOOTER_SIZE (CTDB_I64_LEN * 5) //cksum_1, tran_count, del_count, root_pos, cksum_2

#define CTDB_OK 0
#define CTDB_ERR -1

struct ctdb_footer{
    uint64_t tran_count;
    uint64_t del_count;
    int64_t root_pos;
};

struct ctdb{
    int fd;
    struct ctdb_footer footer;
};

struct ctdb_node{
    uint8_t prefix_len;
    char prefix[CTDB_MAX_KEY_LEN + 1];
    int64_t leaf_pos;
    
    uint8_t items_count;
    struct ctdb_node_item{
        char sub_prefix_char;
        int64_t sub_node_pos;
    }items[CTDB_MAX_CHAR_RANGE];
};

struct ctdb_leaf{
    //int create_time;
    //int expire;
    //int fingerprint;
    uint32_t value_len;
    int64_t value_pos;
};

struct ctdb_transaction{
    uint8_t is_isvalid;
    struct ctdb *db;
    struct ctdb_footer new_footer;
};

//API
struct ctdb *ctdb_open(char *path);
struct ctdb_leaf ctdb_get(struct ctdb *db, char *key, uint8_t key_len);
struct ctdb_transaction *ctdb_transaction_begin(struct ctdb *db);
int ctdb_put(struct ctdb_transaction *trans, char *key, uint8_t key_len, char *value, uint32_t value_len);
int ctdb_del(struct ctdb_transaction *trans, char *key, uint8_t key_len);
int ctdb_transaction_commit(struct ctdb_transaction *trans);
void ctdb_transaction_rollback(struct ctdb_transaction *trans);
void ctdb_transaction_free(struct ctdb_transaction *trans);
void ctdb_close(struct ctdb *db);

//iterator
typedef int ctdb_traversal(int fd, char *key, uint8_t key_len, struct ctdb_leaf leaf);
int ctdb_iterator_travel(struct ctdb *db, char *key, uint8_t key_len, ctdb_traversal *traversal);

#ifdef __cplusplus
}
#endif
#endif
