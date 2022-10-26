# Handle null pointer in Linux Kernel


When it comes to returning an error from functions that are supposed to return a pointer,
functions often return the NULL pointer. It is a working but quite meaningless approach,
since you do not exactly know why this null pointer is returned. For that purpose, the
kernel provides three functions, ERR_PTR , IS_ERR , and PTR_ERR :
``` C
void *ERR_PTR(long error);
long IS_ERR(const void*ptr);
long PTR_ERR(const void *ptr);
```
The first actually returns the error value as a pointer. Given a function that is likely to
return -ENOMEM after a failed memory allocation, we have to do something such
as return ERR_PTR(-ENOMEM); . The second is used to check whether the returned value
is a pointer error or not, if (IS_ERR(foo)) . The last returns the actual error code, return
PTR_ERR(foo); . The following is an example:
This is how to use ERR_PTR , IS_ERR , and PTR_ERR :
``` C
static struct iio_dev*indiodev_setup(){
    [...]
    struct iio_dev *indio_dev;
    indio_dev = devm_iio_device_alloc(&data->client->dev, sizeof(data));
    if (!indio_dev)
        return ERR_PTR(-ENOMEM);
    [...]
    return indio_dev;
}
static int foo_probe([...]){
    [...]
    struct iio_dev*my_indio_dev = indiodev_setup();
    if (IS_ERR(my_indio_dev))
        return PTR_ERR(data->acc_indio_dev);
    [...]
}
```