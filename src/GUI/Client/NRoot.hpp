#ifndef CF_GUI_Client_NRoot_hpp
#define CF_GUI_Client_NRoot_hpp

//////////////////////////////////////////////////////////////////////////////

#include <boost/uuid/uuid.hpp>

#include "Common/CRoot.hpp"

#include "GUI/Client/CNode.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace CF {

namespace Common { class CPath; }

namespace GUI {
namespace Client {

  ////////////////////////////////////////////////////////////////////////////

  /// @brief Client root.
  /// This class is wrapper for @c CF::Common::CRoot class on the client side.
  /// A NRoot object may never have any child. Add them to the
  /// internal @c CRoot componenent instead. It can be obtained by calling
  /// @c root() method.
  class NRoot :
      public CNode
  {
  public:

    typedef boost::shared_ptr<NRoot> Ptr;
    typedef boost::shared_ptr<NRoot const> ConstPtr;

    /// @brief Constructor
    /// @param name Node name
    NRoot(const QString & name);

    /// @brief Gives the icon associated to this node
    /// @return Returns the icon associated to this node
    /// @note This method should be reimplemented by all subclasses.
    virtual QIcon getIcon() const;

    /// @brief Gives the text to put on a tool tip
    /// @return The name of the class.
    virtual QString getToolTip() const;

    /// @brief Gives the CRoot internal shared pointer
    /// @return Returns the CRoot internal shared pointer
    inline CF::Common::CRoot::Ptr root() const
    {
      return m_root;
    }

    /// @brief Gets a child node from the internal CRoot component
    /// @param number Child number.
    /// @return Returns the child, or a null pointer if the number is not
    /// valid.
    CNode::Ptr getNodeFromRoot(CF::Uint number) const;

    /// @brief Checks whether a path is valid.

    /// The path is checked under the internal CRoot component.
    /// @return Returns @c true is the path exists; otherwise, returns
    /// @c false
    bool pathExists() const;

    std::string getUUID() const;

  private :

    /// @brief The internal CRoot component
    CF::Common::CRoot::Ptr m_root;

    boost::uuids::uuid m_uuid;

    /// regists all the signals declared in this class
    static void regist_signals ( Component* self ) {}


  }; // class NRoot

//////////////////////////////////////////////////////////////////////////////

} // namespace Client
} // namespace GUI
} // namespace CF

//////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Client_NRoot_hpp
