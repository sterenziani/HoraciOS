typedef void* mailbox_t;
mailbox_t create_mailbox(char* name);                           // Retorna el buzón creado o el que ya existe con ese nombre, o NULL si no se pudo crear
int write_message(mailbox_t mailbox, char* msg);                // Escribe msg en el buffer del buzón mailbox. Retorna -1 si el buzon no existe o no hay suficiente lugar para escribir
int read_message(mailbox_t mailbox, int bytes, char* dest);     // Lee n bytes del buffer del buźon mailbox. Si no hay suficiente contenido se bloquea hasta que pueda leer esa cantidad. Retorna -1 si el buzon no existe o pidio demasiados bytes.
void destroy_mailbox(mailbox_t mailbox);

typedef void* mutex_t;
mutex_t mutex_create(char* name);
void mutex_lock(mutex_t mutex);
void mutex_unlock(mutex_t mutex);
void mutex_destroy(mutex_t mutex);

typedef void* sem_t;
sem_t sem_create(char* name, int initial_value);
void sem_wait(sem_t sem);
void sem_signal(sem_t sem);
void sem_destroy(sem_t sem);
