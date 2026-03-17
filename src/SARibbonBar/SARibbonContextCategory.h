#ifndef SARIBBONCONTEXTCATEGORY_H
#define SARIBBONCONTEXTCATEGORY_H
#include "SARibbonGlobal.h"
#include "SARibbonCategory.h"
#include <QWidget>

/**
 * \if ENGLISH
 * @brief Class for managing context categories
 * @details This class manages context categories in the ribbon bar, allowing grouping of related category pages
 * \endif
 *
 * \if CHINESE
 * @brief 管理上下文标签的类
 * @details 此类管理 Ribbon 栏中的上下文标签，允许对相关标签页进行分组
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonContextCategory)
public:
	/// Constructor for SARibbonContextCategory
	explicit SARibbonContextCategory(QWidget* parent = nullptr);
	/// Destructor for SARibbonContextCategory
	~SARibbonContextCategory();
	/// Add a category page with title
	SARibbonCategory* addCategoryPage(const QString& title);
	/// Add an existing category page
	void addCategoryPage(SARibbonCategory* category);
	/// Get the number of managed category pages
	int categoryCount() const;

	/// Set the context ID
	void setId(const QVariant& id);
	/// Get the context ID
	QVariant id() const;

	/// Set the context color
	void setContextColor(const QColor color);
	/// Get the context color
	QColor contextColor() const;

	/// Get the context title
	QString contextTitle() const;
	/// Set the context title
	void setContextTitle(const QString& contextTitle);

	/// Get the category page at the specified index
	SARibbonCategory* categoryPage(int index);

	/// Get all SARibbonCategory objects
	QList< SARibbonCategory* > categoryList() const;

	/// Remove a category from management
	bool takeCategory(SARibbonCategory* category);

	/// Check if the context manages this category
	bool isHaveCategory(SARibbonCategory* category) const;
public Q_SLOTS:
	/// Hide the context category
	void hide();

	/// Show the context category
	void show();
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a category page is added to the context
	 * @param category The added category page
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 标签加入上下文
	 * @param category 添加的标签页
	 * \endif
	 */
	void categoryPageAdded(SARibbonCategory* category);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a category page is removed from the context
	 * @param category The removed category page
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 标签从上下文移除
	 * @param category 移除的标签页
	 * \endif
	 */
	void categoryPageRemoved(SARibbonCategory* category);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when the context title changes
	 * @param title The new context title
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 上下文的标题发生改变
	 * @param title 新的上下文标题
	 * \endif
	 */
	void contextTitleChanged(const QString& title);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a category page title changes
	 * @param category The category page that changed
	 * @param title The new title
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 上下文标签维护的标签页名字发生了改变
	 * @param category 发生改变的上下文标签页
	 * @param title 新名字
	 * \endif
	 */
	void categoryTitleChanged(SARibbonCategory* category, const QString& title);
private Q_SLOTS:
	void onCategoryTitleChanged(const QString& title);

protected:
	/// Get the parent widget
	QWidget* parentWidget() const;
	virtual bool eventFilter(QObject* watched, QEvent* e) override;
};

#endif  // SARIBBONCONTEXTCATEGORY_H
