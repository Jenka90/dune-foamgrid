#ifndef DUNE_IDENTITYGRID_INTERSECTIONITERATOR_HH
#define DUNE_IDENTITYGRID_INTERSECTIONITERATOR_HH

/** \file
* \brief The FoamGridLeafIntersectionIterator and FoamGridLevelIntersectionIterator classes
*/

namespace Dune {

/** \brief Iterator over all element neighbors
* \ingroup FoamGrid
* Mesh entities of codimension 0 ("elements") allow to visit all neighbors, where
* a neighbor is an entity of codimension 0 which has a common entity of codimension 1
* These neighbors are accessed via a IntersectionIterator. This allows the implement
* non-matching meshes. The number of neighbors may be different from the number
* of an element!
*/
template<class GridImp>
class FoamGridLeafIntersectionIterator
{
    
    enum {dim=GridImp::dimension};
    
    enum {dimworld=GridImp::dimensionworld};
    
    // The type used to store coordinates
    typedef typename GridImp::ctype ctype;
    
    typedef typename GridImp::HostGridType::template Codim<0>::Entity::LeafIntersectionIterator HostLeafIntersectionIterator;
    
public:
    
    typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;
    typedef typename GridImp::template Codim<1>::Geometry Geometry;
    typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;
    typedef typename GridImp::template Codim<0>::Entity Entity;
    typedef Dune::Intersection<const GridImp, Dune::FoamGridLeafIntersectionIterator> Intersection;
    
    FoamGridLeafIntersectionIterator(const GridImp* identityGrid,
                                         const HostLeafIntersectionIterator& hostIterator)
        : selfLocal_(NULL), neighborLocal_(NULL), intersectionGlobal_(NULL),
          identityGrid_(identityGrid), 
          hostIterator_(hostIterator)
    {}
        
    //! The Destructor
    ~FoamGridLeafIntersectionIterator() {};
    
    //! equality
    bool equals(const FoamGridLeafIntersectionIterator<GridImp>& other) const {
        return hostIterator_ == other.hostIterator_;
    }

    
    //! prefix increment
    void increment() {
        ++hostIterator_;

        // Delete intersection geometry objects, if present
        if (intersectionGlobal_ != NULL) {
            delete intersectionGlobal_;
            intersectionGlobal_ = NULL;
        }
        
        if (selfLocal_ != NULL) {
            delete selfLocal_;
            selfLocal_ = NULL;
        }
        
        if (neighborLocal_ != NULL) {
            delete neighborLocal_;
            neighborLocal_ = NULL;
        }
    }

    //! \brief dereferencing
    const Intersection & dereference() const {
        return reinterpret_cast<const Intersection&>(*this);
    }
    
        
        //! return EntityPointer to the Entity on the inside of this intersection
        //! (that is the Entity where we started this Iterator)
        EntityPointer inside() const {
            return FoamGridEntityPointer<0,GridImp> (identityGrid_, hostIterator_->inside());
        }

    
        //! return EntityPointer to the Entity on the outside of this intersection
        //! (that is the neighboring Entity)
        EntityPointer outside() const {
            return FoamGridEntityPointer<0,GridImp> (identityGrid_, hostIterator_->outside());
        }

    
        //! return true if intersection is with boundary.
        bool boundary () const {
            return hostIterator_->boundary();
        }
    
        
        //! return true if across the edge an neighbor on this level exists
        bool neighbor () const {
            return hostIterator_->neighbor();
        }
        
        
        //! return information about the Boundary
        int boundaryId () const {
            return hostIterator_->boundaryId();
        }

    //! Return true if this is a conforming intersection
    bool conforming () const {
        return hostIterator_->conforming();
    }
        
    //! Geometry type of an intersection
    GeometryType type () const {
        return hostIterator_->type();
    }


        //! intersection of codimension 1 of this neighbor with element where
        //! iteration started.
        //! Here returned element is in LOCAL coordinates of the element
        //! where iteration started.
        const  LocalGeometry& geometryInInside () const {
            if (selfLocal_ == NULL)
                selfLocal_ = new MakeableInterfaceObject<LocalGeometry>(hostIterator_->geometryInInside());
                
            return *selfLocal_;
        }
    
        //! intersection of codimension 1 of this neighbor with element where iteration started.
        //! Here returned element is in LOCAL coordinates of neighbor
        const  LocalGeometry& geometryInOutside () const {
            if (neighborLocal_ == NULL)
                neighborLocal_ = new MakeableInterfaceObject<LocalGeometry>(hostIterator_->geometryInOutside());
                
            return *neighborLocal_;
        }
        
        //! intersection of codimension 1 of this neighbor with element where iteration started.
        //! Here returned element is in GLOBAL coordinates of the element where iteration started.
        const  Geometry& geometry () const {
            if (intersectionGlobal_ == NULL)
                intersectionGlobal_ = new MakeableInterfaceObject<Geometry>(hostIterator_->geometry());
                
            return *intersectionGlobal_;
        }
    
        
        //! local number of codim 1 entity in self where intersection is contained in
        int indexInInside () const {
            return hostIterator_->indexInInside();
        }
    
        
        //! local number of codim 1 entity in neighbor where intersection is contained
        int indexInOutside () const {
            return hostIterator_->indexInOutside();
        }
    
    
        //! return outer normal
        FieldVector<ctype, GridImp::dimensionworld> outerNormal (const FieldVector<ctype, GridImp::dimension-1>& local) const {
            return hostIterator_->outerNormal(local);
        }

        //! return outer normal multiplied by the integration element
        FieldVector<ctype, GridImp::dimensionworld> integrationOuterNormal (const FieldVector<ctype, GridImp::dimension-1>& local) const {
            return hostIterator_->integrationOuterNormal(local);
        }

        //! return unit outer normal
        FieldVector<ctype, GridImp::dimensionworld> unitOuterNormal (const FieldVector<ctype, GridImp::dimension-1>& local) const {
            return hostIterator_->unitOuterNormal(local);
        }
        
    
    private:
        //**********************************************************
        //  private methods
        //**********************************************************

    //! pointer to element holding the selfLocal and selfGlobal information.
    //! This element is created on demand.
    mutable MakeableInterfaceObject<LocalGeometry>* selfLocal_;
    mutable MakeableInterfaceObject<LocalGeometry>* neighborLocal_;
    
    //! pointer to element holding the neighbor_global and neighbor_local
    //! information.
    mutable MakeableInterfaceObject<Geometry>* intersectionGlobal_;

    const GridImp* identityGrid_;

    HostLeafIntersectionIterator hostIterator_;
};




//! \todo Please doc me !
template<class GridImp>
class FoamGridLevelIntersectionIterator
{
    
        enum {dim=GridImp::dimension};
    
        enum {dimworld=GridImp::dimensionworld};
    
        // The type used to store coordinates
        typedef typename GridImp::ctype ctype;
    
    typedef typename GridImp::HostGridType::template Codim<0>::Entity::LevelIntersectionIterator HostLevelIntersectionIterator;
    
    public:

        typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;
        typedef typename GridImp::template Codim<1>::Geometry Geometry;
        typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;
        typedef typename GridImp::template Codim<0>::Entity Entity;
        typedef Dune::Intersection<const GridImp, Dune::FoamGridLevelIntersectionIterator> Intersection;

    FoamGridLevelIntersectionIterator(const GridImp* identityGrid,
                                     const HostLevelIntersectionIterator& hostIterator)
        : selfLocal_(NULL), neighborLocal_(NULL), intersectionGlobal_(NULL),
          identityGrid_(identityGrid), hostIterator_(hostIterator)
    {}

        //! equality
        bool equals(const FoamGridLevelIntersectionIterator<GridImp>& other) const {
            return hostIterator_ == other.hostIterator_;
        }

        
        //! prefix increment
        void increment() {
            ++hostIterator_;

            // Delete intersection geometry objects, if present
            if (intersectionGlobal_ != NULL) {
                delete intersectionGlobal_;
                intersectionGlobal_ = NULL;
            }

            if (selfLocal_ != NULL) {
                delete selfLocal_;
                selfLocal_ = NULL;
            }

            if (neighborLocal_ != NULL) {
                delete neighborLocal_;
                neighborLocal_ = NULL;
            }

        }

    //! \brief dereferencing
    const Intersection & dereference() const {
        return reinterpret_cast<const Intersection&>(*this);
    }

        //! return EntityPointer to the Entity on the inside of this intersection
        //! (that is the Entity where we started this Iterator)
        EntityPointer inside() const {
            return FoamGridEntityPointer<0,GridImp> (identityGrid_, hostIterator_->inside());
        }

        
        //! return EntityPointer to the Entity on the outside of this intersection
        //! (that is the neighboring Entity)
        EntityPointer outside() const {
            return FoamGridEntityPointer<0,GridImp> (identityGrid_, hostIterator_->outside());
        }
        
        
    /** \brief return true if intersection is with boundary.
    */
    bool boundary () const {
        return hostIterator_->boundary();
    }
        
        
    //! return true if across the edge an neighbor on this level exists
    bool neighbor () const {
        return hostIterator_->neighbor();
    }
    
    
    //! return information about the Boundary
    int boundaryId () const {
        return hostIterator_->boundaryId();
    }
        
    //! Return true if this is a conforming intersection
    bool conforming () const {
        return hostIterator_->conforming();
    }
        
    //! Geometry type of an intersection
    GeometryType type () const {
        return hostIterator_->type();
    }


        //! intersection of codimension 1 of this neighbor with element where
        //! iteration started.
        //! Here returned element is in LOCAL coordinates of the element
        //! where iteration started.
        const LocalGeometry& geometryInInside () const {
            if (selfLocal_ == NULL)
                selfLocal_ = new MakeableInterfaceObject<LocalGeometry>(hostIterator_->geometryInInside());
                
            return *selfLocal_;
        }
        
        //! intersection of codimension 1 of this neighbor with element where iteration started.
        //! Here returned element is in LOCAL coordinates of neighbor
        const  LocalGeometry& geometryInOutside () const {
            if (neighborLocal_ == NULL)
                neighborLocal_ = new MakeableInterfaceObject<LocalGeometry>(hostIterator_->geometryInOutside());
                
            return *neighborLocal_;
        }
        
        //! intersection of codimension 1 of this neighbor with element where iteration started.
        //! Here returned element is in GLOBAL coordinates of the element where iteration started.
        const Geometry& geometry () const {
            if (intersectionGlobal_ == NULL)
                intersectionGlobal_ = new MakeableInterfaceObject<Geometry>(hostIterator_->geometry());
                
            return *intersectionGlobal_;
        }
        
        
        //! local number of codim 1 entity in self where intersection is contained in
        int indexInInside () const {
            return hostIterator_->indexInInside();
        }
        
        
        //! local number of codim 1 entity in neighbor where intersection is contained
        int indexInOutside () const {
            return hostIterator_->indexInOutside();
        }
        
          
        //! return outer normal
        FieldVector<ctype, dimworld> outerNormal (const FieldVector<ctype, dim-1>& local) const {
            return hostIterator_->outerNormal(local);
        }

        //! return outer normal multiplied by the integration element
        FieldVector<ctype, dimworld> integrationOuterNormal (const FieldVector<ctype, dim-1>& local) const {
            return hostIterator_->integrationOuterNormal(local);
        }

        //! return unit outer normal
        FieldVector<ctype, dimworld> unitOuterNormal (const FieldVector<ctype, dim-1>& local) const {
            return hostIterator_->unitOuterNormal(local);
        }

    private:

    //! pointer to element holding the selfLocal and selfGlobal information.
    //! This element is created on demand.
    mutable MakeableInterfaceObject<LocalGeometry>* selfLocal_;
    mutable MakeableInterfaceObject<LocalGeometry>* neighborLocal_;
    
    //! pointer to element holding the neighbor_global and neighbor_local
    //! information.
    mutable MakeableInterfaceObject<Geometry>* intersectionGlobal_;
    
    const GridImp* identityGrid_;

    HostLevelIntersectionIterator hostIterator_;

};


}  // namespace Dune

#endif
