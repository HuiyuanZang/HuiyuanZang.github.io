# How to use pointer as parameter of contanier_of 

Let's say, we have struct person and family
``` C
struct person {
    int age;
    int salary;
    char *name;
};

struct family {
    struct person *father;
    struct person *mother;
    int number_of_sons;
    int family_id;
} 

```

Somehow, you can get the pointer of father or mum, 
``` C
struct person *dad =  from_somewhere();
struct person *mum =  from_somewhere();
```

Then, you cann't get their family pointer by 

``` C
struct family *my_family = contanier_of(dad, struct family, father);  

```

However, if you can get the pointer of pointer to father or mum, like

``` C
struct person **dad =  from_somewhere();
struct person **mum =  from_somewhere();
```

Then, you can get their fammily pointer by

``` C
struct family *my_family = contanier_of(dad, struct family, father);  

```
The container_of macro won't work for char *or array members. It
means the first member of container_of must not be a pointer to
another pointer to char nor to array in the structure. In other words, in our
first example with the struct person , it would have been wrong to use
name field to retrieve the containing structure. Giving a pointer to
father or mother as first parameter to container_of should be
wrong, since those members are already pointer fields in the structure.
But what about retrieving the structure holding a pointer with
container_of macro ? Let's have a look to our preceding example with
the struct family structure. If you only have a pointer to father or
mother (that is, if you just have struct person*dad or struct
person *mom ), you cannot use the container_of macro to retrieve the
container struct family . To use it properly, you'll need a pointer to a
pointer to struct person (that is, struct person **dad or struct
person **mom ) and use it like struct family*fam =
container_of(dad, struct family, father); .

