


#include "XCScrolledList.h"

#define kBackgroundZorder 1
#define kContainerZorder 5

XCScrolledList::XCScrolledList()
:m_nMaxColumn(10000)
,m_fDragCallback(nullptr)
,m_bStopTouch(false)
,m_vItems(std::vector<XCListItem *>())
{
    
}

XCScrolledList *XCScrolledList::create(const Size &aContentSize)
{
    XCScrolledList *layer = new XCScrolledList();
    if (layer && layer->initWithXCScrolledList(aContentSize)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool XCScrolledList::initWithXCScrolledList(const Size &aContentSize)
{
    
    if (!XCScrollView::initWithViewSize(aContentSize)) {
        return false;
    }
    
    setDirection(XCScrollView::Direction::VERTICAL);
    
    return true;
}

void XCScrolledList::moveList(int aNumber)
{
    setTouchEnabled(false);
    if (_direction == Direction::VERTICAL) {
        _container->runAction(Sequence::create(EaseSineOut::create(MoveBy::create(0.3, Vec2(0, m_nMoveStepY * aNumber))), CallFunc::create([this](){
            relocateContainer(true);
            _delegate->scrollViewDidScroll(this);
            this->setTouchEnabled(true)
            ; }), nullptr));
    } else if (_direction == Direction::HORIZONTAL) {
        _container->runAction(Sequence::create(EaseSineOut::create(MoveBy::create(0.3, Vec2(m_nMoveStepX * aNumber, 0))), CallFunc::create([this](){
            relocateContainer(true);
            _delegate->scrollViewDidScroll(this);
            this->setTouchEnabled(true); }), nullptr));
    }
}

void XCScrolledList::pushItems(std::vector<XCListItem *> &aItems)
{
    if (aItems.empty()) {
        return;
    }
    
    for (auto item : aItems) {
        _container->addChild(item);
    }
    
    m_vItems.insert(m_vItems.end(), aItems.begin(), aItems.end());
    
    updateContainerSize();
    
    updateItemsPosition();
}

void XCScrolledList::pushItem(XCListItem *aItem)
{
    if (aItem == nullptr) {
        return;
    }
    
    _container->addChild(aItem);
    
    m_vItems.insert(m_vItems.end(), aItem);
    
    updateContainerSize();
    
    updateItemsPosition();
}

void XCScrolledList::removeItem(XCListItem *aItem)
{
    if (aItem == nullptr) {
        return;
    }
    
    auto children = _container->getChildren();
    
    if (std::find(children.begin(), children.end(), aItem) == children.end()) {
        log("Not found item");
        return;
    }
    
    auto itemItr = std::find(m_vItems.begin(), m_vItems.end(), aItem);
    
    if (itemItr != m_vItems.end()) {
        m_vItems.erase(itemItr);
    }
    
    _container->removeChild(aItem);
    
    updateContainerSize();
    
    updateItemsPosition();
}

bool XCScrolledList::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (m_bStopTouch == true) {
        return false;
    }
    
    if (XCScrollView::onTouchBegan(touch, event) == false) {
        return false;
    }
    
    if (m_bItemTouch) {
        
        m_pTouchItem = nullptr;
        
        auto showInScreenItems = getShowInScreenItems();
        
        for (auto item : showInScreenItems) {
            if (item->getTouchItem(touch->getLocation())) {
                m_pTouchItem = item;
                scheduleOnce(schedule_selector(XCScrolledList::onTouchItem), 0.1);
                break;
            }
        }
    }
    
    return true;
}

void XCScrolledList::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (m_bStopTouch == true) {
        m_fItemTouchMove(touch);
    } else {
        XCScrollView::onTouchMoved(touch, event);
        if (m_bItemTouch) {
            if (touch->getLocation().distance(touch->getStartLocation()) > 15 && isScheduled(schedule_selector(XCScrolledList::onTouchItem))) {
                unschedule(schedule_selector(XCScrolledList::onTouchItem));
            }
        }
    }
}

void XCScrolledList::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (m_bStopTouch == true) {
        m_fItemTouchEnd(touch);
        resumeScroll();
    } else {
        XCScrollView::onTouchEnded(touch, event);
    }
    
}

std::vector<XCListItem *> XCScrolledList::getShowInScreenItems()
{
    
    std::vector<XCListItem *> showItems;
    
    auto children = _container->getChildren();
    
    float startY = fabsf(_container->getPositionY());
    float endY = _viewSize.height + startY;
    
    for (auto ingredientIcon : children) {
        float PosY = ingredientIcon->getPositionY();
        if (PosY >= startY && PosY <= endY) {
            showItems.push_back(dynamic_cast<XCListItem *>(ingredientIcon));
        }
    }
    
    return showItems;
}

void XCScrolledList::onTouchItem(float aTime)
{
    if (m_fItemTouchBeagan) {
        m_fItemTouchBeagan(m_pTouchItem);
    }
    pauseScroll();
}

void XCScrolledList::cleanupScrollList()
{
    _container->removeAllChildrenWithCleanup(true);
    m_vItems.clear();
    updateContainerSize();
    updateItemsPosition();
}

void XCScrolledList::updateContainerSize()
{
    
    size_t allItemsCount = _container->getChildrenCount();
    
    size_t rowCount = allItemsCount / m_nMaxColumn < 1 ? 1 : ceilf(allItemsCount / m_nMaxColumn);
    size_t columnCount = allItemsCount > m_nMaxColumn ? m_nMaxColumn : allItemsCount;
    
    float containerWidth = columnCount * m_nMoveStepX;
    float containerHeight = rowCount  * m_nMoveStepY;
    
    _container->setContentSize(Size(containerWidth, containerHeight));
    
}

void XCScrolledList::updateItemsPosition()
{
    
    short column = 1;
    
    float widthDistance = m_nMoveStepX * 0.5;
    float heightDistance = m_nMoveStepY * 0.5;
    
    Vec2 lastPos = Vec2(_container->getContentSize().width, 0);
    
    Vector<Node *>::reverse_iterator itemItr = _container->getChildren().rbegin();
    while (itemItr != _container->getChildren().rend()) {
        
        if (column > m_nMaxColumn) {
            lastPos.y += heightDistance;
            lastPos.x = _container->getContentSize().width;
            column = 1;
        }
        
        float x = lastPos.x - widthDistance;
        float y = lastPos.y + heightDistance;
        if (x < 0 || y < 0) {
            log("error!!! calculation Position wrong！");
        } else {
            (*itemItr)->setPosition(x, y);
        }
        
        lastPos.x -= m_nMoveStepX;
        ++ column;
        ++ itemItr;
        
    }
    
    updateInset();
    
    if (_direction == XCScrollView::Direction::VERTICAL) {
        _container->setPosition(minContainerOffset());
    } else if (_direction == XCScrollView::Direction::HORIZONTAL) {
        _container->setPosition(maxContainerOffset());
    }
}

XCScrolledList::~XCScrolledList()
{
    
}