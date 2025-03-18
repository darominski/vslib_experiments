//! @file
//! @brief File with unit tests of MessageQueue class.
//! @author Dominik Arominski

#include <array>
#include <gtest/gtest.h>
#include <string>

#include "messageQueue.hpp"

using namespace fgc4::utils;

class MessageQueueTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        std::fill(buffer.begin(), buffer.end(), 0.0);
    }

    void TearDown() override
    {
    }

  public:
    std::array<uint8_t, 200> buffer;
};

struct MyHeader
{
    int         value_int;
    double      value_double;
    const char* value_string;
};

//! Checks that a reader queue with void header can be initialized and iteracted with
TEST_F(MessageQueueTest, CreateReaderQueueVoid)
{
    auto rd_queue = createMessageQueue<MessageQueueReader<void>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());
    ASSERT_EQ(rd_queue.getPendingMessageSize(), std::nullopt);   // no message has been sent

    std::array<uint8_t, 200> read_buffer{0};
    auto                     message = rd_queue.read(read_buffer);
    ASSERT_FALSE(message.has_value());
}

//! Checks that a reader queue with a custom header can be initialized and iteracted with
TEST_F(MessageQueueTest, CreateReaderQueueCustom)
{
    auto rd_queue = createMessageQueue<MessageQueueReader<MyHeader>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());
    ASSERT_EQ(rd_queue.getPendingMessageSize(), std::nullopt);   // no message has been sent

    std::array<uint8_t, 200> read_buffer{0};
    auto                     message = rd_queue.read(read_buffer);
    ASSERT_FALSE(message.has_value());
}

//! Checks that a writer queue can be initialized and interacted with by sending empty message
TEST_F(MessageQueueTest, CreateWriteQueueVoid)
{
    auto wrt_queue = createMessageQueue<MessageQueueWriter<void>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());
    ASSERT_NO_THROW(wrt_queue.write({}));   // empty message
}

//! Checks that a writer queue with a custom header can be initialized and iteracted with
TEST_F(MessageQueueTest, CreateWriteQueueCustom)
{
    auto wrt_queue = createMessageQueue<MessageQueueWriter<MyHeader>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());

    ASSERT_NO_THROW(wrt_queue.write({1, 3.14, "text"}, {}));
}

//! Checks that a writer queue can send a message and it is successfully read
TEST_F(MessageQueueTest, WriteAndReadQueueVoid)
{
    auto wrt_queue = createMessageQueue<MessageQueueWriter<void>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());
    auto rd_queue  = createMessageQueue<MessageQueueReader<void>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());

    char const greeting[] = "Hello world from Bare-metal";
    ASSERT_NO_THROW(wrt_queue.write({(uint8_t const*)greeting, sizeof(greeting)}));   // empty message

    ASSERT_EQ(rd_queue.getPendingMessageSize(), sizeof(greeting));
    std::array<uint8_t, 200> read_buffer{0};
    auto                     message = rd_queue.read(read_buffer);
    ASSERT_TRUE(message.has_value());
    const char* received_message = (const char*)message->data();
    ASSERT_TRUE(!strcmp(received_message, greeting));
}

//! Checks that a writer queue can send an empty message and that does not cause issues for the reader
TEST_F(MessageQueueTest, WriteAndReadQueueEmptyMessageVoid)
{
    auto wrt_queue = createMessageQueue<MessageQueueWriter<void>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());
    auto rd_queue  = createMessageQueue<MessageQueueReader<void>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());

    ASSERT_NO_THROW(wrt_queue.write({}));   // empty message

    ASSERT_EQ(rd_queue.getPendingMessageSize(), 0);
    std::array<uint8_t, 200> read_buffer{0};
    auto                     message = rd_queue.read(read_buffer);
    ASSERT_TRUE(message.has_value());
    const char* received_message = (const char*)message->data();
    ASSERT_TRUE(!strcmp(received_message, ""));
}

//! Checks that a writer queue with a custom header can be initialized and iteracted with
TEST_F(MessageQueueTest, WriteAndReadQueueCustom)
{
    auto wrt_queue = createMessageQueue<MessageQueueWriter<MyHeader>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());
    auto rd_queue  = createMessageQueue<MessageQueueReader<MyHeader>>(std::bit_cast<uint8_t*>(&buffer), buffer.size());

    ASSERT_NO_THROW(wrt_queue.write({1, 3.14, "text"}, {}));

    std::array<uint8_t, 200> read_buffer{0};
    auto                     message = rd_queue.read(read_buffer);
    ASSERT_TRUE(message.has_value());
    ASSERT_EQ(message->first.value_int, 1);
    ASSERT_EQ(message->first.value_double, 3.14);
    ASSERT_EQ(message->first.value_string, "text");
}
