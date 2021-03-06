#include "gtest/gtest.h"
#include "domparser/IDOMFactory.h"
#include "domparser/PageDataFactory.h"
#include "domparser/IPageData.h"
#include "domparser/PageDataImpl.h"

#include <memory>

TEST(CreateObjectTest, CreateObject)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData(""));

    EXPECT_EQ(pageData, nullptr);
}

TEST(EmptyFileTest, EmptyFile)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index1.html"));

    EXPECT_EQ(pageData->getNumberOfTags(), 0);
    EXPECT_EQ(pageData->first(), nullptr);
    EXPECT_EQ(pageData->last(), nullptr);
    EXPECT_EQ(pageData->next(), nullptr);
    EXPECT_EQ(pageData->prev(), nullptr);
    EXPECT_EQ(pageData->parent(), nullptr);
    EXPECT_EQ(pageData->current(), nullptr);
    EXPECT_TRUE(pageData->children().empty());
    EXPECT_TRUE(pageData->siblings().empty());
    EXPECT_TRUE(pageData->getTagContent().empty());
    EXPECT_TRUE(pageData->getAttributeValue("").empty());
    EXPECT_FALSE(pageData->insertAttribute("", ""));
    EXPECT_FALSE(pageData->changeAttribute("", "", "", ""));;
    EXPECT_FALSE(pageData->removeAttribute("", ""));
    EXPECT_FALSE(pageData->changeContent(""));
    EXPECT_FALSE(pageData->removeContent());
    EXPECT_FALSE(pageData->removeTag());
}

TEST(TestNumberOfTags, NumberOfTags)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    EXPECT_EQ(pageData->getNumberOfTags(), 10);
}

TEST(TestCurrentTagNumber, CurrentTagNumber)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    pageData->setCurrentTag(5);

    EXPECT_EQ(pageData->getCurrentTagNumber(), 5);
}

TEST(NavigationTest, Valid)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    EXPECT_EQ(pageData->first()->getTagName(), "html");
    EXPECT_EQ(pageData->last()->getTagName(), "i");
    EXPECT_EQ(pageData->next()->getTagName(), "script");
    EXPECT_EQ(pageData->prev()->getTagName(), "html");
}

TEST(NavigationTest, Children)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    auto children = pageData->children();

    EXPECT_EQ(children.size(), 4);
    EXPECT_EQ(children[0]->getTagName(), "script");
    EXPECT_EQ(children[1]->getTagName(), "style");
    EXPECT_EQ(children[2]->getTagName(), "head");
    EXPECT_EQ(children[3]->getTagName(), "body");
}

TEST(NavigationTest, Siblings)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    pageData->setCurrentTag(1);
    auto siblings = pageData->siblings();

    EXPECT_EQ(siblings.size(), 3);
    EXPECT_EQ(siblings[0]->getTagName(), "style");
    EXPECT_EQ(siblings[1]->getTagName(), "head");
    EXPECT_EQ(siblings[2]->getTagName(), "body");
}

TEST(ModificationTest, InsertAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    EXPECT_TRUE(pageData->first()->getAttributeTag().empty());
    EXPECT_TRUE(pageData->first()->getAttributeValueTag().empty());

    pageData->insertAttribute("class", "name");

    auto attribute = pageData->first()->getAttributeTag();
    auto attributeValue = pageData->first()->getAttributeValueTag();

    EXPECT_EQ(attribute.size(), 1);
    EXPECT_EQ(attributeValue.size(), 1);
    EXPECT_EQ(attribute[0], "class");
    EXPECT_EQ(attributeValue[0], "name");
}

TEST(ModificationTest, ChangeAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "class");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameCl");

    pageData->changeAttribute("class", "nameCl", "size", "10");

    EXPECT_EQ(pageData->current()->getAttributeTag().size(), 1);
    EXPECT_EQ(pageData->current()->getAttributeValueTag().size(), 1);
    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "size");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "10");
}

TEST(ModificationTest, ChangeAttribute_NoNeedAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "class");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameCl");
    EXPECT_FALSE(pageData->changeAttribute("id", "nameCl", "size", "10"));
}

TEST(ModificationTest, ChangeAttribute_NoNeedAttributeValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "class");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameCl");
    EXPECT_FALSE(pageData->changeAttribute("class", "name", "size", "10"));
}

TEST(ModificationTest, ChangeAttribute_NoNeedAttributeWithNeedValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "name");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameI");
    EXPECT_FALSE(pageData->changeAttribute("name", "2", "size", "10"));
}

TEST(ModificationTest, RemoveAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "class");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameCl");

    pageData->removeAttribute("class", "nameCl");

    EXPECT_TRUE(pageData->current()->getAttributeTag().empty());
    EXPECT_TRUE(pageData->current()->getAttributeValueTag().empty());
}

TEST(ModificationTest, RemoveAttribute_NoNeedAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "class");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameCl");
    EXPECT_FALSE(pageData->removeAttribute("id", "nameCl"));
}

TEST(ModificationTest, RemoveAttribute_NoNeedAttributeValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "class");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameCl");
    EXPECT_FALSE(pageData->removeAttribute("class", "name"));
}

TEST(ModificationTest, RemoveAttribute_NoNeedAttributeWithAttributeValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getAttributeTag().at(0), "name");
    EXPECT_EQ(pageData->current()->getAttributeValueTag().at(0), "nameI");
    EXPECT_FALSE(pageData->removeAttribute("class", "2"));
}

TEST(ModificationTest, PushBack)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div");

    EXPECT_EQ(pageData->getNumberOfTags(), 10);
    EXPECT_EQ(pageData->last()->getTagName(), "i");

    pageData->pushBack(*tag.get());

    EXPECT_EQ(pageData->getNumberOfTags(), 11);
    EXPECT_EQ(pageData->last()->getTagName(), "div");
}

TEST(ModificationTest, PushFront)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div");

    EXPECT_EQ(pageData->getNumberOfTags(), 10);
    EXPECT_EQ(pageData->first()->getTagName(), "html");

    pageData->pushFront(*tag.get());

    EXPECT_EQ(pageData->getNumberOfTags(), 11);
    EXPECT_EQ(pageData->first()->getTagName(), "div");
}

TEST(ModificationTest, PushBefore)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(5);

    EXPECT_EQ(pageData->current()->getTagName(), "body");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushBefore(*pageData->current(), *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "div1");
    EXPECT_EQ(pageData->next()->getTagName(), "body");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushBefore2)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");

    EXPECT_EQ(pageData->current()->getTagName(), "html");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushBefore(*pageData->current(), *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "div1");
    EXPECT_EQ(pageData->next()->getTagName(), "html");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushBefore3)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getTagName(), "i");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushBefore(*pageData->current(), *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "div1");
    EXPECT_EQ(pageData->next()->getTagName(), "i");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushBeforeIndex)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(5);

    EXPECT_EQ(pageData->current()->getTagName(), "body");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushBefore(5, *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "div1");
    EXPECT_EQ(pageData->next()->getTagName(), "body");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushBeforeIndex2)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");

    EXPECT_EQ(pageData->current()->getTagName(), "html");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushBefore(0, *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "div1");
    EXPECT_EQ(pageData->next()->getTagName(), "html");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushBeforeIndex3)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getTagName(), "i");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushBefore(9, *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "div1");
    EXPECT_EQ(pageData->next()->getTagName(), "i");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushAfter)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(5);

    EXPECT_EQ(pageData->current()->getTagName(), "body");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushAfter(*pageData->current(), *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "body");
    EXPECT_EQ(pageData->next()->getTagName(), "div1");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushAfter2)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");

    EXPECT_EQ(pageData->current()->getTagName(), "html");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushAfter(*pageData->current(), *tag.get());

    EXPECT_EQ(pageData->first()->getTagName(), "html");
    EXPECT_EQ(pageData->current()->getTagName(), "html");
    EXPECT_EQ(pageData->next()->getTagName(), "div1");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushAfter3)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getTagName(), "i");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushAfter(*pageData->current(), *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "i");
    EXPECT_EQ(pageData->next()->getTagName(), "div1");
    EXPECT_EQ(pageData->last()->getTagName(), "div1");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushAfterIndex)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(5);

    EXPECT_EQ(pageData->current()->getTagName(), "body");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushAfter(5, *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "body");
    EXPECT_EQ(pageData->next()->getTagName(), "div1");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushAfterIndex2)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");

    EXPECT_EQ(pageData->current()->getTagName(), "html");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushAfter(0, *tag.get());

    EXPECT_EQ(pageData->first()->getTagName(), "html");
    EXPECT_EQ(pageData->current()->getTagName(), "html");
    EXPECT_EQ(pageData->next()->getTagName(), "div1");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, PushAfterIndex3)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    std::unique_ptr<Tag> tag(new Tag);
    tag->setTagName("div1");
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getTagName(), "i");
    EXPECT_EQ(pageData->getNumberOfTags(), 10);

    pageData->pushAfter(9, *tag.get());

    EXPECT_EQ(pageData->current()->getTagName(), "i");
    EXPECT_EQ(pageData->next()->getTagName(), "div1");
    EXPECT_EQ(pageData->last()->getTagName(), "div1");
    EXPECT_EQ(pageData->getNumberOfTags(), 11);
}

TEST(ModificationTest, ChangeContent)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getTagName(), "div");
    EXPECT_EQ(pageData->current()->getContent(), "Block Content");

    pageData->changeContent("Another content");

    EXPECT_EQ(pageData->current()->getContent(), "Another content");
}

TEST(ModificationTest, RemoveContent)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(6);

    EXPECT_EQ(pageData->current()->getTagName(), "div");
    EXPECT_EQ(pageData->current()->getContent(), "Block Content");

    pageData->removeContent();

    EXPECT_TRUE(pageData->current()->getContent().empty());
}

TEST(ModificationTest, RemoveFirstTag)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    EXPECT_EQ(pageData->current()->getTagName(), "html");

    pageData->removeTag();

    EXPECT_EQ(pageData->current()->getTagName(), "script");
}

TEST(ModificationTest, RemoveLastTag)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->current()->getTagName(), "i");

    pageData->removeTag();

    EXPECT_EQ(pageData->current()->getTagName(), "p");
}

TEST(ModificationTest, RemoveTag)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(5);

    EXPECT_EQ(pageData->current()->getTagName(), "body");

    pageData->removeTag();

    EXPECT_EQ(pageData->current()->getTagName(), "div");
}

TEST(GetValue, GetTagName)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));

    EXPECT_EQ(pageData->getTagName(), "html");
}

TEST(GetValue, GetTagContent)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->getTagContent(), "Content");
}

TEST(GetValue, GetAttributeValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html"));
    pageData->setCurrentTag(9);

    EXPECT_EQ(pageData->getAttributeValue("size"), "2");
}

TEST(InterfaceRuleTest, SelectDiv)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "div"));

    EXPECT_EQ(pageData->getNumberOfTags(), 1);
    EXPECT_EQ(pageData->current()->getTagName(), "div");
    EXPECT_EQ(pageData->current()->getAttributeTag().size(), 1);
    EXPECT_EQ(pageData->current()->getAttributeValueTag().size(), 1);
}

TEST(InterfaceRuleTest, SelectAllWithAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[name]"));

    EXPECT_EQ(pageData->getNumberOfTags(), 3);
    EXPECT_EQ(pageData->first()->getTagName(), "p");
    EXPECT_EQ(pageData->next()->getTagName(), "p");
    EXPECT_EQ(pageData->last()->getTagName(), "i");
}

TEST(InterfaceRuleTest, SelectAllWithAttributeAndValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[class='nameCl']"));

    EXPECT_EQ(pageData->getNumberOfTags(), 1);
    EXPECT_EQ(pageData->first()->getTagName(), "div");
}

TEST(InterfaceRuleTest, SelectAllWithEndString)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[background$='.png']"));

    EXPECT_EQ(pageData->getNumberOfTags(), 1);
    EXPECT_EQ(pageData->first()->getTagName(), "body");
}

TEST(InterfaceRuleTest, SelectAllNotEqualAttributeValue)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[name!='nameI']"));

    EXPECT_EQ(pageData->getNumberOfTags(), 2);
    EXPECT_EQ(pageData->first()->getTagName(), "p");
    EXPECT_EQ(pageData->first()->getContent(), "Text");
    EXPECT_EQ(pageData->last()->getTagName(), "p");
    EXPECT_EQ(pageData->last()->getContent(), "Another text");
}

TEST(InterfaceRuleTest, SelectAllWithBeginString)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[name^='name']"));

    EXPECT_EQ(pageData->getNumberOfTags(), 3);
    EXPECT_EQ(pageData->first()->getTagName(), "p");
    EXPECT_EQ(pageData->next()->getTagName(), "p");
    EXPECT_EQ(pageData->last()->getTagName(), "i");
}

TEST(InterfaceRuleTest, SelectAllWithPartString)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[name*='am']"));

    EXPECT_EQ(pageData->getNumberOfTags(), 3);
    EXPECT_EQ(pageData->first()->getTagName(), "p");
    EXPECT_EQ(pageData->next()->getTagName(), "p");
    EXPECT_EQ(pageData->last()->getTagName(), "i");
}

TEST(InterfaceRuleTest, SelectTagsWithMatchingAttributes)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "[name=\"nameI\"],[size=\"2\"],[with=\"6\"]"));

    EXPECT_EQ(pageData->getNumberOfTags(), 1);
    EXPECT_EQ(pageData->first()->getTagName(), "i");
}

TEST(InterfaceRuleTest, SelectSpecificTagWithSpecifiedAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "p[name=\"nameP\"]"));

    EXPECT_EQ(pageData->getNumberOfTags(), 2);
    EXPECT_EQ(pageData->first()->getTagName(), "p");
    EXPECT_EQ(pageData->last()->getTagName(), "p");
}

TEST(InterfaceRuleTest, SelectChildrenTagWithAttribute)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "body[background=\"picture.png\"][size=\"2\"]"));

    EXPECT_EQ(pageData->getNumberOfTags(), 1);
    EXPECT_EQ(pageData->first()->getTagName(), "i");
}

TEST(InterfaceRuleTest, SelectChildrenOfTheSpecificTag)
{
    std::unique_ptr<IDOMFactory> ptr(new PageDataFactory);
    std::unique_ptr<IPageData> pageData(ptr->createPageData("index.html", "body > p"));

    EXPECT_EQ(pageData->getNumberOfTags(), 2);
    EXPECT_EQ(pageData->first()->getTagName(), "p");
    EXPECT_EQ(pageData->last()->getTagName(), "p");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

