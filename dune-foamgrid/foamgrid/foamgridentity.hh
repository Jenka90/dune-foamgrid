#ifndef DUNE_IDENTITYGRIDENTITY_HH
#define DUNE_IDENTITYGRIDENTITY_HH

/** \file
* \brief The FoamGridEntity class
*/

#include <dune/grid/common/referenceelements.hh>


namespace Dune {


// Forward declarations

template<int codim, int dim, class GridImp>
class FoamGridEntity;

template<int codim, class GridImp>
class FoamGridEntityPointer;

template<int codim, PartitionIteratorType pitype, class GridImp>
class FoamGridLevelIterator;

template<class GridImp>
class FoamGridLevelIntersectionIterator;

template<class GridImp>
class FoamGridLeafIntersectionIterator;

template<class GridImp>
class FoamGridHierarchicIterator;




template<int codim, int dim, class GridImp>
class FoamGridMakeableEntity :
    public GridImp::template Codim<codim>::Entity
{

    enum {dimworld = GridImp::dimensionworld};

    public:

        //! \todo Please doc me !
    FoamGridMakeableEntity(const FoamGridEntityImp<dim-codim,dimworld>* target) :
            GridImp::template Codim<codim>::Entity (FoamGridEntity<codim, dim, const GridImp>(target))
        {}
        
        
        //! \todo Please doc me !
    void setToTarget(const FoamGridEntityImp<dim-codim,dimworld>* target) {
            this->realEntity.setToTarget(target);
        }
        
        
        //! \todo Please doc me !
    const FoamGridEntityImp<dim-codim,dimworld>* getTarget() {
            return this->realEntity.target_;
        }
    
};


//**********************************************************************
//
// --FoamGridEntity
// --Entity
//
/** \brief The implementation of entities in a FoamGrid
*   \ingroup FoamGrid
*
*  A Grid is a container of grid entities. An entity is parametrized by the codimension.
*  An entity of codimension c in dimension d is a d-c dimensional object.
*
*/
template<int codim, int dim, class GridImp>
class FoamGridEntity :
    public EntityDefaultImplementation <codim,dim,GridImp,FoamGridEntity>
{
    friend class FoamGridMakeableEntity<codim,dim,GridImp>;

    template <class GridImp_>
    friend class FoamGridLevelIndexSet;

    template <class GridImp_>
    friend class FoamGridLeafIndexSet;

    template <class GridImp_>
    friend class FoamGridLocalIdSet;

    template <class GridImp_>
    friend class FoamGridGlobalIdSet;

    template <class GridImp_, int EntityDim>
    friend class IndexSetter;

    friend class FoamGridEntityPointer<codim,GridImp>;

    
    private:
        
        typedef typename GridImp::ctype ctype;
        
    public:
    
        typedef typename GridImp::template Codim<codim>::Geometry Geometry;
    
        
        //! Constructor for an entity in a given grid level
    FoamGridEntity(const FoamGridEntityImp<dim-codim,GridImp::dimensionworld>* target) :
            target_(target),
            geo_(0),
            geoInFather_(0)
        {}
        
        //! \todo Please doc me !
        FoamGridEntity(const FoamGridEntity& original) :
            target_(original.target_),
            geo_(0),
            geoInFather_(0)
        {}
    
        
        //! Destructor
        ~FoamGridEntity()
        {
            if (geo_!=0)
            {
                delete geo_;
                geo_ = 0;
            }
            if (geoInFather_!=0)
            {
                delete geoInFather_;
                geoInFather_ = 0;
            }
        }
        
        
        //! \todo Please doc me !
        FoamGridEntity& operator=(const FoamGridEntity& original)
        {
            if (this != &original)
            {
                if (geo_!=0)
                {
                    delete geo_;
                    geo_ = 0;
                }
                if (geoInFather_!=0)
                {
                    delete geoInFather_;
                    geoInFather_ = 0;
                }
                target_ = original.target_;
            }
            return *this;
        }
    
    
        //! level of this element
        int level () const {
            return target_->level();
        }
    
        
        /** \brief The partition type for parallel computing
        */
        PartitionType partitionType () const {
            return target_->partitionType();
        }
    
        
        /** Intra-element access to entities of codimension cc > codim. Return number of entities
        * with codimension cc.
        */
        template<int cc> int count () const{
            return target_->template count<cc>();
        }
        
        
        //! geometry of this entity
        const Geometry& geometry () const
        {
            if (geo_==0)
                geo_ = new MakeableInterfaceObject<Geometry>(target_->geometry());
            return *geo_;
        }
    
    FoamGridEntityImp<dim-codim,GridImp::dimensionworld>* target_;

        
    private:
    
        //! \todo Please doc me !
        void setToTarget(const FoamGridEntityImp<dim-codim,GridImp::dimensionworld>* target)
        {
            if(geo_!=0)
            {
                delete geo_;
                geo_ = 0;
            }
            if (geoInFather_!=0)
            {
                delete geoInFather_;
                geoInFather_ = 0;
            }
            target_ = target;
        }
    
        
        //! the current geometry
    mutable MakeableInterfaceObject<Geometry> *geo_;
    mutable MakeableInterfaceObject<Geometry> *geoInFather_;
};




//***********************
//
//  --FoamGridEntity
//
//***********************
/** \brief Specialization for codim-0-entities.
* \ingroup FoamGrid
*
* This class embodies the topological parts of elements of the grid.
* It has an extended interface compared to the general entity class.
* For example, Entities of codimension 0  allow to visit all neighbors.
*/
template<int dim, class GridImp>
class FoamGridEntity<0,dim,GridImp> :
    public EntityDefaultImplementation<0,dim,GridImp, FoamGridEntity>
{
    public:
    
        typedef typename GridImp::template Codim<0>::Geometry Geometry;
    
        typedef typename GridImp::template Codim<0>::LocalGeometry LocalGeometry;
    
        //! The Iterator over intersections on this level
        typedef FoamGridLevelIntersectionIterator<GridImp> LevelIntersectionIterator;
    
        //! The Iterator over intersections on the leaf level
        typedef FoamGridLeafIntersectionIterator<GridImp> LeafIntersectionIterator;
    
        //! Iterator over descendants of the entity
        typedef FoamGridHierarchicIterator<GridImp> HierarchicIterator;
        
        
        //! Constructor for an entity in a given grid level
        FoamGridEntity(const FoamGridElement* hostEntity) :
            geo_(0),
            geoInFather_(0),
            target_(hostEntity)
        {}
        
        
        //! \todo Please doc me !
        FoamGridEntity(const FoamGridEntity& original) :
            geo_(0),
            geoInFather_(0),
            target_(original.target_)
        {}
    
        
        //! Destructor
        ~FoamGridEntity()
        {
            if (geo_!=0)
            {
                delete geo_;
                geo_ = 0;
            }
            if (geoInFather_!=0)
            {
                delete geoInFather_;
                geoInFather_ = 0;
            }
        }
        
        
        //! \todo Please doc me !
        FoamGridEntity& operator=(const FoamGridEntity& original)
        {
            if (this != &original)
            {
                if (geo_!=0)
                {
                    delete geo_;
                    geo_ = 0;
                }
                if (geoInFather_!=0)
                {
                    delete geoInFather_;
                    geoInFather_ = 0;
                }
                target_ = original.target_;
            }
            return *this;
        }
    
        
        //! Level of this element
        int level () const
        {
            return target_->level_;
        }
    
        
        /** \brief The partition type for parallel computing */
        PartitionType partitionType () const {
            return InteriorEntity;
        }
    
        
        //! Geometry of this entity
        const Geometry& geometry () const
        {
            if (geo_==0)
                geo_ = new MakeableInterfaceObject<Geometry>(FoamGridGeometry<dim,GridImp::dimensionworld,GridImp>());

            std::vector<FieldVector<double, GridImp::dimensionworld> > coordinates(target_->corners());
            for (size_t i=0; i<target_->corners(); i++)
                coordinates[i] = target_->vertex_[i]->pos_;

            GridImp::getRealImplementation(*geo_).setup(target_->type(), coordinates);
            return *geo_;
        }
    
        
        /** \brief Return the number of subEntities of codimension cc.
        */
        template<int cc>
        int count () const
        {
            dune_static_assert(0<=cc && cc<=2, "Only codimensions with 0 <= cc <= 2 are valid!");
            return (cc==0) ? 1 : 3;
        }
        
    /** \brief Return level index of sub entity with codim = cc and local number i
     */
    int subLevelIndex (int i,unsigned int codim) const {
        assert(0<=codim && codim<=dim);
        switch (codim) {
        case 0:
            return target_->levelIndex_;
        case 1:
            return target_->edges_[i]->levelIndex_;
        case 2:
            return target_->vertex_[i]->levelIndex_;
        }
        DUNE_THROW(GridError, "Non-existing codimension requested!");
    }

    /** \brief Return leaf index of sub entity with codim = cc and local number i
     */
    int subLeafIndex (int i,unsigned int codim) const {
        assert(0<=codim && codim<=dim);
        switch (codim) {
        case 0:
            return target_->leafIndex_;
        case 1:
            return target_->edges_[i]->leafIndex_;
        case 2:
            return target_->vertex_[i]->leafIndex_;
        }
        DUNE_THROW(GridError, "Non-existing codimension requested!");
    }

    /** \brief Return index of sub entity with codim = cc and local number i
     */
    int subId (int i,unsigned int codim) const {
        assert(0<=codim && codim<=dim);
        switch (codim) {
        case 0:
            return target_->id_;
        case 1:
            return target_->edges_[i]->id_;
        case 2:
            return target_->vertex_[i]->id_;
        }
        DUNE_THROW(GridError, "Non-existing codimension requested!");
    }
    
        

        /** \brief Provide access to sub entity i of given codimension. Entities
        *  are numbered 0 ... count<cc>()-1
        */
        template<int cc>
        typename GridImp::template Codim<cc>::EntityPointer subEntity (int i) const{
            assert(false);
            //return FoamGridEntityPointer<cc,GridImp>(hostEntity_->template subEntity<cc>(i));
        }
    
        
        //! First level intersection
        FoamGridLevelIntersectionIterator<GridImp> ilevelbegin () const{
            assert(false);
            //return FoamGridLevelIntersectionIterator<GridImp>(hostEntity_->ilevelbegin());
        }
    
        
        //! Reference to one past the last neighbor
        FoamGridLevelIntersectionIterator<GridImp> ilevelend () const{
            assert(false);
            //return FoamGridLevelIntersectionIterator<GridImp>(hostEntity_->ilevelend());
        }
    
        
        //! First leaf intersection
        FoamGridLeafIntersectionIterator<GridImp> ileafbegin () const{
            assert(false);
//             return FoamGridLeafIntersectionIterator<GridImp>(identityGrid_,
            //hostEntity_->ileafbegin());
        }
    
        
        //! Reference to one past the last leaf intersection
        FoamGridLeafIntersectionIterator<GridImp> ileafend () const{
            assert(false);
//             return FoamGridLeafIntersectionIterator<GridImp>(identityGrid_,
            //                                                   hostEntity_->ileafend());
        }
    
        
        //! returns true if Entity has NO children
        bool isLeaf() const {
            return target_->isLeaf();
        }
    
        
        //! Inter-level access to father element on coarser grid.
        //! Assumes that meshes are nested.
        FoamGridEntityPointer<0,GridImp> father () const {
            return FoamGridEntityPointer<0,GridImp>(target_->father_);
        }
    
    
        /** \brief Location of this element relative to the reference element element of the father.
        * This is sufficient to interpolate all dofs in conforming case.
        * Nonconforming may require access to neighbors of father and
        * computations with local coordinates.
        * On the fly case is somewhat inefficient since dofs  are visited several times.
        * If we store interpolation matrices, this is tolerable. We assume that on-the-fly
        * implementation of numerical algorithms is only done for simple discretizations.
        * Assumes that meshes are nested.
        */
        const LocalGeometry& geometryInFather () const {
            DUNE_THROW(NotImplemented, "geometryInFather");
//             if (geoInFather_==0)
//                 geoInFather_ = new MakeableInterfaceObject<LocalGeometry>(hostEntity_->geometryInFather());
            return *geoInFather_;
        }
    
        
        /** \brief Inter-level access to son elements on higher levels<=maxlevel.
        * This is provided for sparsely stored nested unstructured meshes.
        * Returns iterator to first son.
        */
        FoamGridHierarchicIterator<GridImp> hbegin (int maxLevel) const
        {
            //return FoamGridHierarchicIterator<const GridImp>(identityGrid_, *this, maxLevel);
        }
    
        
        //! Returns iterator to one past the last son
        FoamGridHierarchicIterator<GridImp> hend (int maxLevel) const
        {
            //return FoamGridHierarchicIterator<const GridImp>(identityGrid_, *this, maxLevel, true);
        }
        
        
        // /////////////////////////////////////////
        //   Internal stuff
        // /////////////////////////////////////////
    
        
        //! \todo Please doc me !
        void setToTarget(const FoamGridElement* target)
        {
            if(geo_!=0)
            {
                delete geo_;
                geo_ = 0;
            }
            if (geoInFather_!=0)
            {
                delete geoInFather_;
                geoInFather_ = 0;
            }
            target_ = target;
        }
        
        //! the current geometry
        mutable MakeableInterfaceObject<Geometry> *geo_;
        
        //! \todo Please doc me !
        mutable MakeableInterfaceObject<LocalGeometry> *geoInFather_;

    const FoamGridElement* target_;
        
    private:
    
        typedef typename GridImp::ctype ctype;

}; // end of FoamGridEntity codim = 0


} // namespace Dune


#endif
