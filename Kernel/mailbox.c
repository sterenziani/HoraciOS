#include <lib.h>
#include <mailbox.h>
#include <memoryManager.h>
#include <strings.h>
#include <naiveConsole.h>
#include <scheduler.h>
#include <mutex.h>

static post_office_t post_office;

void init_post_office()
{
  post_office.free_index = 0;
  for(int i = 0; i < MAX_MAILBOXES; i++)
    post_office.mailboxes[i] = NULL;
}

int get_mailbox_index(char* mailbox)
{
  for (int i = 0; i < MAX_MAILBOXES; i++)
  {
    if(post_office.mailboxes[i] != NULL && strcmp(post_office.mailboxes[i]->name, mailbox) == 0)
      return i;
  }
  return -1;
}

mailbox_t create_mailbox(char* name)
{
  // Check if already exists
  int index = get_mailbox_index(name);
  if(index != -1)
    return post_office.mailboxes[index];

  // Let's create one if allowed to
  if(post_office.free_index == -1)
    return NULL;
  mailbox_t new_mailbox = malloc(sizeof(mailbox_struct_t));
  if(new_mailbox == NULL)
    return NULL;

  // Tenemos espacio para crear un buzon
  int length = strlen(name);
  if(length > NAME_LENGTH)
    length = NAME_LENGTH;
  for (int i = 0; i < length; i++)
    new_mailbox->name[i] = name[i];
  new_mailbox->name[length] = 0;

  new_mailbox->read_index = 0;
  new_mailbox->write_index = 0;
  new_mailbox->empty_bytes = MAILBOX_BUFFER_SIZE;
  for (int i = 0; i < MAILBOX_BUFFER_SIZE; i++)
    new_mailbox->buffer[i] = 0;
  new_mailbox->queue_index = 0;
  post_office.mailboxes[post_office.free_index] = new_mailbox;

  // Actualizo el free_index
  int newly_created_index = post_office.free_index;
  post_office.free_index++;
  if(post_office.free_index == MAX_MAILBOXES)
    post_office.free_index = 0;
  while(post_office.mailboxes[post_office.free_index] != NULL && post_office.free_index != newly_created_index)
  {
      post_office.free_index++;
      if(post_office.free_index >= MAX_MAILBOXES)
        post_office.free_index -= MAX_MAILBOXES;
  }
  if(post_office.free_index == newly_created_index)
    post_office.free_index = -1;
  return new_mailbox;
}

int readable_bytes(mailbox_t mailbox)
{
  if(mailbox == NULL)
    return -1;
  return MAILBOX_BUFFER_SIZE - mailbox->empty_bytes;
}

int writable_bytes(mailbox_t mailbox)
{
  if(mailbox == NULL)
    return -1;
  return mailbox->empty_bytes;
}

// Pide al scheduler que bloquee el proceso activo y lo meta en la lista de espera del mailbox
void block_process(mailbox_t mailbox)
{
  process_id_t process = get_current_pid();
  // Add current process to my list and tell scheduler to block it
  if(mailbox->queue_index == PROCESS_QUEUE_SIZE)
    print("ERROR! Too many processes waiting for mailbox!");
  mailbox->lockedQueue[mailbox->queue_index] = process;
  mailbox->queue_index++;
  mark_process_as_blocked(process);
}

// Pide al scheduler que despierte todo proceso de su lista de espera y los borra de la lista
void wake_up_processes(mailbox_t mailbox)
{
  // Tell scheduler to wake up everybody in my list
  for (int i = 0; i < mailbox->queue_index; i++)
    resume_process(mailbox->lockedQueue[i]);
  mailbox->queue_index = 0;
}

int write_message(mailbox_t mailbox, char* msg)
{
  if(mailbox == NULL)
    return ERROR;
  if(writable_bytes(mailbox) < strlen(msg))
    return ERROR;

  mutex_t mailbox_mutex = mutex_create("OS_mailbox_mutex");
  mutex_lock(mailbox_mutex);
  // strcpy
  int write_index = mailbox->write_index;
  int length = strlen(msg);
  for (int i = 0; i < length; i++)
  {
    mailbox->buffer[(write_index + i)%MAILBOX_BUFFER_SIZE] = msg[i];
    mailbox->empty_bytes--;
  }
  // Update write_index
  mailbox->write_index += length;
  mailbox->write_index = (mailbox->write_index % MAILBOX_BUFFER_SIZE);
  wake_up_processes(mailbox);
  mutex_unlock(mailbox_mutex);
  return SUCCESS;
}

int write_message_by_name(char* name, char* msg)
{
  int index = get_mailbox_index(name);
  if(index == -1)
    return ERROR;
  mailbox_t mailbox = post_office.mailboxes[index];
  return write_message(mailbox, msg);
}

int read_message(mailbox_t mailbox, int bytes, char* dest)
{
  if(mailbox == NULL)
    return ERROR;
  if(bytes > MAILBOX_BUFFER_SIZE)
    return ERROR;

  mutex_t mailbox_mutex = mutex_create("OS_mailbox_mutex");
  mutex_lock(mailbox_mutex);
  while(readable_bytes(mailbox) < bytes)
  {
    //printFormat("Blocking", ORANGE_COLOR, BACKGROUND_COLOR);
    //newLine();
    mutex_unlock(mailbox_mutex);
    block_process(mailbox);
    //printFormat("Woke up", ORANGE_COLOR, BACKGROUND_COLOR);
    mutex_lock(mailbox_mutex);
  }
  // Let's read
  for (int i = 0; i < bytes; i++)
  {
    dest[i] = mailbox->buffer[(mailbox->read_index + i)%MAILBOX_BUFFER_SIZE];
    mailbox->empty_bytes++;
  }
  dest[bytes] = 0;

  // Update read_index
  mailbox->read_index += bytes;
  mailbox->read_index = (mailbox->read_index % MAILBOX_BUFFER_SIZE);
  mutex_unlock(mailbox_mutex);
  return SUCCESS;
}

int read_message_by_name(char* name, int bytes, char* dest)
{
  int index = get_mailbox_index(name);
  if(index == -1)
    return ERROR;
  mailbox_t mailbox = post_office.mailboxes[index];
  return read_message(mailbox, bytes, dest);
}

void destroy_mailbox(mailbox_t mailbox)
{
  destroy_mailbox_by_name(mailbox->name);
}

void destroy_mailbox_by_name(char* name)
{
  int index = get_mailbox_index(name);
  if(index == -1)
    return;
  mailbox_t mailbox = post_office.mailboxes[index];
  free_handler(mailbox);
  post_office.mailboxes[index] = NULL;
  post_office.free_index = index;
}
