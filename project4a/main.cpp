#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <iostream>
#include <sstream>
#include "buffer.h"

using namespace std;

buffer_item buffer[BUFFER_SIZE];

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

int bufferCurrent;

void *producer(void *arguments);
void *consumer(void *arguments);
void init();

void init()
{
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, BUFFER_SIZE);
  pthread_mutex_init(&mutex, NULL);
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  bufferCurrent = 0;

  for(int temp = 0; temp<BUFFER_SIZE; temp++)
  {
    buffer[temp] = 0;
  }
}

int insert_item(buffer_item item)
{
  if(bufferCurrent < BUFFER_SIZE)
  {
    buffer[bufferCurrent] = item;
    bufferCurrent++;
    return 0;
  }
  else
  {
    return -1;
  }
}

int remove_item(buffer_item *item)
{
  if(bufferCurrent > 0)
  {
    *item = buffer[(bufferCurrent-1)];
    bufferCurrent--;
    return 0;
  }
  else
  {
    return -1;
  }
}

void *producer(void *arguments)
{
  buffer_item item;

  while(1)
  {
    usleep(rand());
    item = (rand());

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    int temp = insert_item(item);
    if(temp != 0)
    {
       cout << "Insert failed" << endl;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
  return 0;
}

void *consumer(void *arguments)
{
  buffer_item item;

  while(1)
  {
    usleep(rand());
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    int temp = remove_item(&item);
    if(temp != 0)
    {
       cout << "Removal failed" << endl;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }

  return 0;
}


int main(int argc, char *argv[])
{



  int sleepTime;
  int numProducers;
  int numConsumers;

/*
  if(argc != 4)
  {
    cout << "Please enter the sleep time, number of producer threads, and number of consumer threads when executing." << endl;
    return -1;
  }


  int temp;
  istringstream ss(argv[1]);
  ss >> temp;
  sleepTime = temp;

  istringstream ss2(argv[2]);
  ss2 >> temp;
  numProducers = temp;

  istringstream ss3(argv[3]);
  ss3 >> temp;
  numConsumers = temp;
  */

  sleepTime = 10;
  numProducers = 10;
  numConsumers = 10;

  cout << "Established values" << endl;

  init();

  cout << "Finished initilization" << endl;

  int rc;
  int i;
  pthread_t prodThreads[numProducers];
  pthread_t consThreads[numConsumers];
  pthread_attr_t attr;
  void *status;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for( i=0; i < numProducers; i++ )
  {
      rc = pthread_create(&prodThreads[i], NULL, &producer, NULL);
      if (rc)
      {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   cout << "Created Producers" << endl;

   for( i=0; i < numConsumers; i++ )
  {
      rc = pthread_create(&consThreads[i], NULL, &consumer, NULL);
      if (rc)
      {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

  cout << "Created Consumers" << endl;

  pthread_attr_destroy(&attr);

  usleep(sleepTime);

  cout << "Finished." << endl;
}
