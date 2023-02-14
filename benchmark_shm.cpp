#include <benchmark/benchmark.h>

#include <xt/core/utility/shm/shm.hpp>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <iostream>
#include <thread>



struct shared_data {
    /// Поле будет в первой кэш линии.
    int one{0};
    /// Заполнитель для того, что бы следующее поле было точно в другой кэш
    /// линии.
    char hole[64];
    /// Второе поле. Оно гарантировано будет в другой кэш линии.
    int two{0};
} data;


using namespace boost::interprocess;

static void shm_boost_thread (benchmark::State& state) {

    /// таймер выполнения кода
    for (auto _ : state) {

        boost::interprocess::shared_memory_object object_write1 (open_or_create, "boost", read_write);
        object_write1.truncate(sizeof(shared_data));
        mapped_region region_write{object_write1, read_write};
        shared_data *data_write = static_cast<shared_data*>(region_write.get_address());

        shared_memory_object object_read1 (open_or_create, "boost", read_write);
        object_read1.truncate(sizeof(shared_data));
        mapped_region region_read{object_read1, read_write};
        shared_data *data_read = static_cast<shared_data*>(region_read.get_address());

        shared_memory_object object_write2 (open_or_create, "boost", read_write);
        object_write2.truncate(sizeof(shared_data));
        mapped_region region_write2{object_write2, read_write};
        shared_data *data_write2 = static_cast<shared_data*>(region_write2.get_address());

        shared_memory_object object_read2 (open_or_create, "boost", read_write);
        object_read2.truncate(sizeof(shared_data));
        mapped_region region_read2{object_read2, read_write};
        shared_data *data_read2 = static_cast<shared_data*>(region_read2.get_address());


        std::thread t1([&](){
            /// Пройдем нужное количество раз данные.
            for (int i = 0; i < 1'000'000; ++i) {
                /// пишущий объект пишет
                ++data_write->one;
                /// объект читает от пишущего во втором потоке
                int two = data_read->two;
            }
        });

        std::thread t2([&](){

            /// Пройдем нужное количество раз данные.
            for (int i = 0; i < 1'000'000; ++i) {
                /// пишет для объекта из потока один
                ++data_write2->two;
                /// читает от объекта из потока один
                int one = data_read2->one;
            }
        });
        t1.join();
        t2.join();

        shared_memory_object::remove("boost_write_read");
        shared_memory_object::remove("boost_read_write");
    }
}


static void shm_sistemv_thread (benchmark::State& state) {

    /// таймер выполнения кода
    for (auto _ : state) {
        /// объект пишущий для читающего объекта во втором потоке
        xt::shm::system_v::object<shared_data, 4> object_write1("object.shm", true);
        /// объект читающий от пишущего объекта во втором потоке
        xt::shm::system_v::object<shared_data, 4> object_read1("object.shm", false);

        /// объект читающий от пишущего сервера
        xt::shm::system_v::object<shared_data, 4> object_read2("object.shm", false);
        /// объект пишущий для читающего сервера
        xt::shm::system_v::object<shared_data, 4> object_write2("object.shm", false);

    //for (auto _ : state) {
        /// поток один
        std::thread t1([&](){
            /// Пройдем нужное количество раз данные.
            for (int i = 0; i < 1'000'000; ++i) {
                /// пишущий объект пишет
                ++object_write1->one;
                /// объект читает от пишущего во втором потоке
                int two = object_read1->two;
            }
        });

        /// поток два
        std::thread t2([&](){
            /// Пройдем нужное количество раз данные.
            for (int i = 0; i < 1'000'000; ++i) {
                /// пишет для объекта из потока один
                ++object_write2->two;
                /// читает от объекта из потока один
                int one = object_read2->one;
            }
        });
        t1.join();
        t2.join();
    }
}


static void shm_posix_thread (benchmark::State& state) {

    /// таймер выполнения кода
    for (auto _ : state) {
        /// объект пишущий для читающего объекта во втором потоке
        xt::shm::posix::object<shared_data> object_write1("posix", true);
        /// объект читающий от пишущего объекта во втором потоке
        xt::shm::posix::object<shared_data> object_read1("posix", false);

        /// объект читающий от пишущего сервера
        xt::shm::posix::object<shared_data> object_read2("posix", false);
        /// объект пишущий для читающего сервера
        xt::shm::posix::object<shared_data> object_write2("posix", false);

        /// поток один
        std::thread t1([&](){
            /// Пройдем нужное количество раз данные.
            for (int i = 0; i < 1'000'000; ++i) {
                /// пишущий объект пишет
                ++object_write1->one;
                /// объект читает от пишущего во втором потоке
                int two = object_read1->two;
            }
        });

        /// поток два
        std::thread t2([&](){
            /// Пройдем нужное количество раз данные.
            for (int i = 0; i < 1'000'000; ++i) {
                /// пишет для объекта из потока один
                ++object_write2->two;
                /// читает от объекта из потока один
                int one = object_read2->one;
            }
        });

        t1.join();
        t2.join();
    }
}


/// Register the function as a benchmark
BENCHMARK(shm_sistemv_thread);
BENCHMARK(shm_posix_thread);
BENCHMARK(shm_boost_thread);

BENCHMARK_MAIN();






