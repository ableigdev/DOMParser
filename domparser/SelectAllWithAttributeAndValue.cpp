#include "SelectAllWithAttributeAndValue.h"

SelectAllWithAttributeAndValue::SelectAllWithAttributeAndValue(const std::cmatch& cm)
: m_Match(cm.begin(), cm.end())
{
}

bool SelectAllWithAttributeAndValue::checkRules(Tag* tag) const
{
    if (tag != nullptr)
    {
        auto attribute = tag->getAttributeTag();
        auto attributeValue = tag->getAttributeValueTag();

        if (attribute.size() == attributeValue.size())
        {
            for (size_t i = 0; i < attribute.size(); ++i)
            {
                if (attribute[i] == m_Match[2] && attributeValue[i] == m_Match[3])
                {
                    return true;
                }
            }
        }
    }
    return false;
}