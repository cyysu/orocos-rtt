#ifndef PI_PROPERTY_SEQUENCE_HPP
#define PI_PROPERTY_SEQUENCE_HPP

#include "PropertyBase.hpp"

#ifdef HAVE_VECTOR
#include <vector>
#endif

#ifdef HAVE_ALGORITHM
#include <algorithm>
#endif

#ifdef HAVE_IOSTREAM
#include <iostream>
#else
#include "rtstl/rtstl.hpp"
#endif

namespace ORO_CoreLib
{
	/**
	 * A container for a sequence of properties of the same type Property<T>.
     * 
	 * This class groups properties of equal types into a single
	 * container. A PropertyBag object can handed to a Marshaller object
	 * which will serialize the contents of the PropertyBag.
     *
     * All operations on a PropertyBag are non recursive. The PropertyBag
     * itself is not aware of the possible hierarchical structure. You need
     * to use the global scope functions for handling hierarchical structures.
     *
     * A PropertyBag is not the owner of the properties within. It defines
     * a group of properties, nothing more. When this group of properties
     * contains another PropertyBag, this bag is considered as a 'pointer'
     * to other properties. Even such a bag is not owned by the PropertyBag.
     *
     * Constructing PropertyBags.
     * It can happen in an applycation that a PropertyBag is filled with
     * dynamically created Property objects. The bag is not responsible for
     * cleaning up these objects once they are no longer needed. The application
     * which uses the PropertyBag has to find a way of deleting the possible
     * dangling pointers.
     *
     * The elements in a PropertyBag are <em>unordered</em>. Operations on the
     * bag may change the relative order of the elements.
     * 
	 * @see PropertyBase
	 */
    class PropertyBag
    {
        public:
			typedef std::vector<PropertyBase*> PropertyContainerType;
			typedef PropertyContainerType::iterator iterator;
            typedef PropertyContainerType::const_iterator const_iterator;

			/**
			 * The default constructor.
			 */
            PropertyBag( )
                : _properties(), type("type_less")
            {}

			/**
			 * The typed constructor.
             *
             * @param type The type of PropertyBag.
			 */
            PropertyBag( const std::string& _type)
                : _properties(), type(_type)
            {}

            /**
             * The copy constructor.
             * The copy constructor of the PropertyBag makes
             * non-deep copies of its elements.
             */
            PropertyBag( const PropertyBag& orig)
                : _properties( orig.getProperties() ), type( orig.getType() )
            {
#if 0
                const_iterator i( orig.getProperties().begin() );
                while ( i != orig.getProperties().end() )
                {
                    add( (*i) );
                    ++i;
                }
#endif
            }

			/**
			 * Add a property to the container.
			 * @param p Pointer to the property to be added.
			 */
            void add(PropertyBase *p)
            {
                _properties.push_back(p);
            }
            
			/**
			 * Remove a property from the container.
			 * @param p Pointer to the property to be removed.
			 */
			void remove(PropertyBase *p)
            {
                iterator i = _properties.begin();
                i = _properties.end();
				i = std::find(_properties.begin(), _properties.end(), p);
                if ( i != _properties.end() )
                    _properties.erase(i);
            }

            /**
             * Removes all PropertyBases from this bag, without deleting
             * them. This is a not recursive function.
             */
            void clear()
            {
                _properties.clear();
            }
                

			/**
			 * List all properties.
			 * @param names The container which will be used to store all
			 * available property names in.
			 */
            void list(std::vector<std::string> &names) const
            {
                for (
                    const_iterator i = _properties.begin();
                    i != _properties.end();
                    i++ )
                {
					names.push_back( (*i)->getName() );
                }
            }

            /**
             * Find the PropertyBase with name <name>.
             *
             * @param  name The name of the property to search for.
             * @return The PropertyBase with this name, zero
             *         if it does not exist.
             */
            PropertyBase* find(const std::string& name) const
            {
				const_iterator i( std::find_if(_properties.begin(), _properties.end(), std::bind2nd(FindProp(), name ) ) );
                if ( i != _properties.end() )
                    return ( *i );
                return 0;
            }
            
            /**
             * This assignment assigns all
             * PropertyBases of another bag in this bag, making
             * an exact copy or the original.
             */
            PropertyBag& operator=(const PropertyBag& orig)
            {
                _properties.clear();

                const_iterator i = orig.getProperties().begin();
                while (i != orig.getProperties().end() )
                {
                    add( (*i) );
                    ++i;
                }
                return *this;
            }

            /**
             * The update operator. 
             * It updates this bag so it contains all PropertyBases
             * of another bag, removing own PropertyBases if duplicate names
             * exist in the source bag.
             */
            PropertyBag& operator<<=(const PropertyBag& source)
            {
                //iterate over orig, update or clone PropertyBases
                const_iterator it(source.getProperties().begin());
                while ( it != source.getProperties().end() )
                {
                    PropertyBase* mine = find( (*it)->getName() );
                    if (mine != 0)
                        remove(mine);
                    add( (*it) );
                    ++it;
                }
                return *this;
            }
            
            const std::string& getType() const { return type;}

			const PropertyContainerType& getProperties() const { return _properties; }

        protected:
			PropertyContainerType _properties;

            /**
             * A function object for finding a Property by name.
             */
            struct FindProp : public std::binary_function<const PropertyBase*,const std::string, bool>
            {
                bool operator()(const PropertyBase* b1, const std::string& b2) const { return b1->getName() == b2; }
            };

            const std::string type;
    };


    /**
     * Some helper functions to retrieve PropertyBases from
     * PropertyBags.
     *
     * @param bag The bag to look for a Property.
     * @param nameSequence A sequence of names, separated by
     *        a double colon indicating the path in the bag to a property,
     *        omitting the name of the <bag> itself.
     */
    PropertyBase* find(const PropertyBag& bag, const std::string& nameSequence, const std::string& separator = std::string("::") );


    /**
     * @group BagOperations These functions operate on the contents of bags,
     *        possibly modifying, deleting or creating new Property objects.
     * @{
     */
    
    /**
     * This function refreshes the values of the properties in one PropertyBag with
     * the values of the properties of another PropertyBag.
     * No new properties will be created.
     * 
     * You can use this function to update the properties of a fixed bag.
     */
    void refreshProperties(PropertyBag& target, const PropertyBag& source);
    
    /**
     * This function updates the values of Property objects of one Bag with the 
     * values of Property objects of another bag. 
     * It creates new Property instances using if a Property
     * is not present in the target and class copy on that Property.
     * 
     * You can use this function to add a copy of the contents of a property bag.
     */
    void copyProperties(PropertyBag& target, const PropertyBag& source);
        
    /**
     * This function iterates over a PropertyBag and deletes all Property objects in
     * it without recursion.
     */
    void deleteProperties(PropertyBag& target);

    /**
     * This function flattens a PropertyBag recursively.
     * The names of the Propety objects of the included bags are
     * placed in this bag with the included bag's name prefixed. If the Property object
     * in that bag is also a bag, the same operation is performed recursively.
     * So any bag in <target> will show up at the
     * root of <target> with the path prefixed.
     */
    void flattenPropertyBag(PropertyBag& target, const std::string& separator="::");
    
    /**
     * @}
     */

    /**
     * Updating a bag is actually refreshing the bag.
     */
    inline
    void update(PropertyBag& a, const PropertyBag& b)
    {
        refreshProperties(a,b);
    }

    /**
     * Copying a bag is actually making a deep copy of the bag.
     */
    inline
    void copy(PropertyBag& a, const PropertyBag& b)
    {
        std::cout << "Copy Bag "<<std::endl;
        copyProperties(a,b);
    }

} // Namespace ORO_CoreLib
#endif
