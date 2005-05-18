#ifndef HASHCACHE_HH
#define HASHCACHE_HH

template <typename T>
class HashCache {
public:
  HashCache(void);
  inline bool insert(int key, T item, T *removed);
  inline bool find(int key, T *result);
  void set_max_items(int max);
  int get_num_items(void) { return num_items; }
  bool remove_last_item(T *removed);
  bool remove_item(int key, T *removed);

private:
  void rehash(int new_max);
  inline int get_hash_index(int key) { return (key%hash_table_size); }
  
  class StoreType {
  public:
    T value;
    int key;
    StoreType *hash_list_next;
    StoreType **hash_list_prev;

    StoreType *priority_list_next;
    StoreType *priority_list_prev;
  };
  std::vector< StoreType* > *hash_table;

  StoreType *first, *last;

  int hash_table_size;
  int num_items;
  int max_num_items;
};

template<typename T>
HashCache<T>::HashCache(void)
{
  hash_table = NULL;
  hash_table_size = num_items = 0;
  max_num_items = 0;
  first = last = NULL;
}

template<typename T>
void
HashCache<T>::rehash(int new_max)
{
  std::vector< StoreType* > *new_hash_table = new std::vector< StoreType* >;
  new_hash_table->resize(new_max);
  for (int i = 0; i < new_max; i++)
    (*new_hash_table)[i] = NULL;
  if (hash_table != NULL)
  {
    // Copy at most new_max items
    // FIXME: Missing! We just delete the old hash.
    assert( num_items == 0 );
    delete hash_table;
  }
  hash_table = new_hash_table;
  hash_table_size = new_max;
  num_items = 0;
  max_num_items = new_max;
}

// Returns true if an item is removed, in which case the removed item is
// stored to variable *removed (if it is != NULL).
template<typename T>
bool
HashCache<T>::insert(int key, T item, T *removed)
{
  int index = get_hash_index(key);
  bool rm = false;
  StoreType *temp;
  
  if (num_items >= max_num_items)
  {
    // Remove the last item
    if (last->hash_list_next != NULL)
      last->hash_list_next->hash_list_prev = last->hash_list_prev;
    *(last->hash_list_prev) = last->hash_list_next;
    if (removed != NULL)
      *removed = last->value;
    temp = last->priority_list_prev;
    delete last;
    last = temp;
    last->priority_list_next = NULL;
    rm = true;
  }
  else
    num_items++;
  temp = new StoreType;
  temp->key = key;
  temp->value = item;
  temp->hash_list_next = (*hash_table)[index];
  temp->hash_list_prev = &(*hash_table)[index];
  if ((*hash_table)[index] != NULL)
    (*hash_table)[index]->hash_list_prev = &temp->hash_list_next;
  (*hash_table)[index] = temp;
  temp->priority_list_prev = NULL;
  temp->priority_list_next = first;
  if (first != NULL)
    first->priority_list_prev = temp;
  else
    last = temp;
  first = temp;
  return rm;
}

template<typename T>
bool
HashCache<T>::remove_last_item(T *removed)
{
  StoreType *temp;
  // Remove the last item
  if (last != NULL)
  {
    if (last->hash_list_next != NULL)
      last->hash_list_next->hash_list_prev = last->hash_list_prev;
    *(last->hash_list_prev) = last->hash_list_next;
    if (removed != NULL)
      *removed = last->value;
    temp = last->priority_list_prev;
    delete last;
    last = temp;
    if (last != NULL)
      last->priority_list_next = NULL;
    else
      first = NULL;
    num_items--;
    return true;
  }
  return false;
}

template<typename T>
bool
HashCache<T>::remove_item(int key, T *removed)
{
  int index = get_hash_index(key);
  StoreType *temp;

  temp = (*hash_table)[index];
  while (temp != NULL)
  {
    if (temp->key == key)
    {
      if (removed != NULL)
        *removed = temp->value;
      if (temp->hash_list_next != NULL)
        temp->hash_list_next->hash_list_prev = temp->hash_list_prev;
      *(temp->hash_list_prev) = temp->hash_list_next;
      if (temp->priority_list_next != NULL)
        temp->priority_list_next->priority_list_prev=temp->priority_list_prev;
      else
        last = temp->priority_list_prev; // temp was last
      if (temp->priority_list_prev != NULL)
        temp->priority_list_prev->priority_list_next=temp->priority_list_next;
      else
        first = temp->priority_list_next; // temp was first
      delete temp;
      num_items--;
      
      return true;
    }
    temp = temp->hash_list_next;
  }
  return false;
}

template<typename T>
bool
HashCache<T>::find(int key, T *result)
{
  int index = get_hash_index(key);
  StoreType *temp;

  temp = (*hash_table)[index];
  while (temp != NULL)
  {
    if (temp->key == key)
    {
      *result = temp->value;
      return true;
    }
    temp = temp->hash_list_next;
  }
  return false;
}


template<typename T>
void
HashCache<T>::set_max_items(int max)
{
  rehash(max);
}


#endif // HASHCACHE_HH
