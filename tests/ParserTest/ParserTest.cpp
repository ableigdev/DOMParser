#include "gtest/gtest.h"

#include "domparser/BaseParser.h"
#include "domparser/ContentParser.h"
#include "domparser/ProcessPage.h"
#include "domparser/TagNameParser.h"
#include "domparser/DataParser.h"
#include "domparser/AttributeParser.h"
#include "domparser/AttributeValueParser.h"


#include <memory>

TEST(Test2, Verify)
{
    ProcessPage processPage("index.html");
    processPage.process();

    std::vector<Tag> pageData = processPage.getPageData();

    EXPECT_EQ(pageData.size(), 8);
    EXPECT_EQ(pageData[0].getTagName(), "html");
    EXPECT_EQ(pageData[0].getParent(), nullptr);
    EXPECT_EQ(pageData[1].getParent()->getTagName(), "html");
}


TEST(ParserNameTeg, ValidCase)
{
    std::string inputData("<html><head><Title>Caption</Title></head></html>");
    std::unique_ptr<BaseParser> ptr(new TagNameParser(inputData));
    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ("html", result[0].getTagName());
    EXPECT_EQ(result.size(), 1);
}

TEST(ParserNameTeg, InvalidCase)
{
    std::string inputData("html>head><Title>Caption</Title></head></html>");
    std::vector<std::string> expectResult;
    expectResult.emplace_back("Title");
    std::unique_ptr<BaseParser> ptr(new TagNameParser(inputData));
    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(expectResult[0], result[0].getTagName());
}

TEST(ParserAttribute, ValidCase)
{
    std::string inputData("<body><div class=\"name\" with ='2'>Content</div><i>str</i><b size = \"2\">str2</b></body>");
    std::unique_ptr<BaseParser> ptr(new AttributeParser(inputData));
    std::vector<std::string> expectResult;
    expectResult.emplace_back("class");
    expectResult.emplace_back("with ");
    expectResult.emplace_back("size ");

    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ(expectResult.size(), result.size());
    EXPECT_EQ(expectResult[0], result[0].getAttribute());
    EXPECT_EQ(expectResult[1], result[1].getAttribute());
    EXPECT_EQ(expectResult[2], result[2].getAttribute());
}

TEST(ParserAttribute, InvalidCase)
{
    std::string inputData("<div class\"name\" with ~'2'>/div>");
    std::unique_ptr<BaseParser> ptr(new AttributeParser(inputData));
    std::vector<DataParser> result = ptr->parse();

    EXPECT_TRUE(result.empty());
}

TEST(ParserAttributeValue, ValidCase)
{
    std::string inputData("<body width=4><div class=\"name\" with = '2'>Content</div><i id='someid'>str</i><b size = \"5\">str2</b></body>");
    std::unique_ptr<BaseParser> ptr(new AttibuteValueParser(inputData));
    std::vector<std::string> expectResult;

    expectResult.emplace_back("4");
    expectResult.emplace_back("name");
    expectResult.emplace_back("2");
    expectResult.emplace_back("someid");
    expectResult.emplace_back("5");

    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ(expectResult.size(), result.size());
    EXPECT_EQ(expectResult[0], result[0].getAttributeValue());
    EXPECT_EQ(expectResult[1], result[1].getAttributeValue());
    EXPECT_EQ(expectResult[2], result[2].getAttributeValue());
    EXPECT_EQ(expectResult[3], result[3].getAttributeValue());
}

TEST(ParseAttributeValue, InvalidCase)
{
    std::string inputData("<body width=\"4'><div class=name\" with 2></div><i id='someid\">str</i><b size ~ 5>str2</b></body>");
    std::unique_ptr<BaseParser> ptr(new AttibuteValueParser(inputData));

    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ(result[0].getAttributeValue(), "");
    EXPECT_EQ(result.size(), 1);
}

TEST(ContentParser, ValidCase)
{
    std::string inputData("<html><head><Title>Caption</Title></head></html>");
    std::unique_ptr<BaseParser> ptr(new ContentParser(inputData));

    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].getContent(), "<head><Title>Caption</Title></head>");
    EXPECT_EQ(result[0].getTagName(), "html");
    EXPECT_EQ(result[0].getNotParsingAttributes(), "");
}

TEST(ContentParser, InvalidCase)
{
    std::string inputData("<html></html>");
    std::unique_ptr<BaseParser> ptr(new ContentParser(inputData));

    std::vector<DataParser> result = ptr->parse();

    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].getContent(), "");
    EXPECT_EQ(result[0].getTagName(), "html");
    EXPECT_EQ(result[0].getNotParsingAttributes(), "");
}

TEST(ContentParser, InvalidCase2)
{
    std::string inputData("html>Content</html>");
    std::unique_ptr<BaseParser> ptr(new ContentParser(inputData));
    std::vector<DataParser> result {};

    EXPECT_NO_THROW(result = ptr->parse());
    EXPECT_EQ(result.size(), 0);
    EXPECT_TRUE(result.empty());
}

TEST(ContentParser, InvalidCase3)
{
    std::string inputData("<html>Content<html>");
    std::unique_ptr<BaseParser> ptr(new ContentParser(inputData));
    std::vector<DataParser> result {};

    EXPECT_NO_THROW(result = ptr->parse());
    EXPECT_EQ(result.size(), 0);
    EXPECT_TRUE(result.empty());
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
