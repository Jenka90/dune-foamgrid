#ifndef DUNE_FOAMGRID_INDEXSETS_HH
#define DUNE_FOAMGRID_INDEXSETS_HH

/** \file
* \brief The index and id sets for the FoamGrid class
*/

#include <vector>

namespace Dune {

    /** \todo Take the index types from the host grid */
    template<class GridImp>
    class FoamGridLevelIndexSet :
        public IndexSet<GridImp,FoamGridLevelIndexSet<GridImp> >
    {

        enum {dim = GridImp::dimension};

    public:
        
        //! get index of an entity
        template<int codim>
        int index (const typename GridImp::Traits::template Codim<codim>::Entity& e) const
        {
            return grid_->getRealImplementation(e).levelIndex();
        }
        
        
        //! get index of subEntity of a codim 0 entity
        int subIndex (const typename GridImp::Traits::template Codim<0>::Entity& e, int i, int codim) const
        {
            return grid_->getRealImplementation(e).subLevelIndex(i,codim);
        }
        
        
        //! get number of entities of given codim, type and on this level
        int size (int codim) const {
            return grid_->size(level_,codim);
        }
        
        
        //! get number of entities of given codim, type and on this level
        int size (GeometryType type) const
        {
            return grid_->size(level_,type);
        }
        
        
        /** \brief Deliver all geometry types used in this grid */
        const std::vector<GeometryType>& geomTypes (int codim) const
        {
            return myTypes_[codim];
        }

        /** \brief Return true if the given entity is contained in the index set */
        template<class EntityType>
        bool contains (const EntityType& e) const
        {
            DUNE_THROW(NotImplemented, "contains()");
        }
        
        /** \brief Set up the index set */
        void update(const GridImp& grid, int level)
        {
            grid_  = &grid;
            level_ = level;

            // ///////////////////////////////
            //   Init the element indices
            // ///////////////////////////////
            numElements_ = 0;
            std::list<FoamGridElement>::const_iterator eIt;
            for (eIt =  Dune::get<dim>(grid_->entityImps_[level_]).begin(); 
                 eIt != Dune::get<dim>(grid_->entityImps_[level_]).end(); 
                 ++eIt)
             /** \todo Remove this const cast */
                 *const_cast<unsigned int*>(&(eIt->levelIndex_)) = numElements_++;
            
            // //////////////////////////////
            //   Init the vertex indices
            // //////////////////////////////
            
            numVertices_ = 0;
            std::list<FoamGridVertex>::const_iterator vIt;
            for (vIt =  Dune::get<0>(grid_->entityImps_[level_]).begin(); 
                 vIt != Dune::get<0>(grid_->entityImps_[level_]).end(); 
                 ++vIt)
                /** \todo Remove this const cast */
                *const_cast<unsigned int*>(&(vIt->levelIndex_)) = numVertices_++;
            
            // ///////////////////////////////////////////////
            //   Update the list of geometry types present
            // ///////////////////////////////////////////////
            if (numElements_>0) {
                myTypes_[0].resize(1);
                myTypes_[0][0] = GeometryType(1);
            } else
                myTypes_[0].resize(0);
            
            if (numVertices_>0) {
                myTypes_[1].resize(1);
                myTypes_[1][0] = GeometryType(0);
            } else
                myTypes_[1].resize(0);
        }
        
        
        GridImp* grid_;
        
        int level_;

        int numElements_;

        int numEdges_;

        int numVertices_;

        /** \brief The GeometryTypes present for each codim */
        std::vector<GeometryType> myTypes_[3];
    };


template<class GridImp>
class FoamGridLeafIndexSet :
    public IndexSet<GridImp,FoamGridLeafIndexSet<GridImp> >
{

    // Grid dimension
    enum {dim      = remove_const<GridImp>::type::dimension};
    
    // Grid dimension
    enum {dimworld = remove_const<GridImp>::type::dimensionworld};
    
public:
    
        //! constructor stores reference to a grid and level
        FoamGridLeafIndexSet (const GridImp& grid)
            : grid_(&grid)
        {}
        
        
        //! get index of an entity
        /*
            We use the RemoveConst to extract the Type from the mutable class,
            because the const class is not instantiated yet.
        */
        template<int codim>
        int index (const typename remove_const<GridImp>::type::template Codim<codim>::Entity& e) const
        {
            return grid_->getRealImplementation(e).target_->leafIndex_; 
        }
        
        
        //! get index of subEntity of a codim 0 entity
        /*
            We use the RemoveConst to extract the Type from the mutable class,
            because the const class is not instantiated yet.
        */
    int subIndex (const typename remove_const<GridImp>::type::Traits::template Codim<0>::Entity& e, 
                  int i, 
                  int codim) const
    {
        return grid_->getRealImplementation(e).subLeafIndex(i,codim);
    }
        
        
    //! get number of entities of given type
    int size (GeometryType type) const
    {
        return (type.dim() < 0 || type.dim() > dim) ? 0 : size_[type.dim()];
    }
        
        
        //! get number of entities of given codim
        int size (int codim) const
        {
            return (codim < 0 || codim > dim) ? 0 : size_[dim - codim];
        }
        
        
        /** \brief Deliver all geometry types used in this grid */
        const std::vector<GeometryType>& geomTypes (int codim) const
        {
            return myTypes_[codim];
        }

    /** \brief Return true if the given entity is contained in the index set */
    template<class EntityType>
    bool contains (const EntityType& e) const
    {
        DUNE_THROW(NotImplemented, "contains");
    }


        
    /** Recompute the leaf numbering */
    void update(const GridImp& grid)
    {

        dune_static_assert(dim==2, "LeafIndexSet::update() only works for 2d grids");

        // ///////////////////////////////
        //   Init the element indices
        // ///////////////////////////////
        size_[dim] = 0;
        typename GridImp::Traits::template Codim<0>::LeafIterator eIt    = grid_->template leafbegin<0>();
        typename GridImp::Traits::template Codim<0>::LeafIterator eEndIt = grid_->template leafend<0>();
        
        for (; eIt!=eEndIt; ++eIt)
            *const_cast<unsigned int*>(&(GridImp::getRealImplementation(*eIt).target_->leafIndex_)) = size_[dim]++;

        // //////////////////////////////
        //   Init the edge indices
        // //////////////////////////////
        
        size_[1] = 0;
        
        for (int i=grid_->maxLevel(); i>=0; i--) {

            typename GridImp::Traits::template Codim<1>::LevelIterator edIt    = grid_->template lbegin<1>(i);
            typename GridImp::Traits::template Codim<1>::LevelIterator edEndIt = grid_->template lend<1>(i);
        
            for (; edIt!=edEndIt; ++edIt) {
                
                const FoamGridEntityImp<1,dimworld>* target = GridImp::getRealImplementation(*edIt).target_;

                // Only implemented for 1-level grids
                assert(target->isLeaf());
                //if (target->isLeaf())
                    *const_cast<unsigned int*>(&(target->leafIndex_)) = size_[1]++;
//                 else
//                     *const_cast<unsigned int*>(&(target->leafIndex_)) = target->son_->leafIndex_;

            }

        }

        // //////////////////////////////
        //   Init the vertex indices
        // //////////////////////////////
        
        size_[0] = 0;
        
        for (int i=grid_->maxLevel(); i>=0; i--) {

            typename GridImp::Traits::template Codim<dim>::LevelIterator vIt    = grid_->template lbegin<dim>(i);
            typename GridImp::Traits::template Codim<dim>::LevelIterator vEndIt = grid_->template lend<dim>(i);
        
            for (; vIt!=vEndIt; ++vIt) {
                
                const FoamGridEntityImp<0,dimworld>* target = GridImp::getRealImplementation(*vIt).target_;

                if (target->isLeaf())
                    *const_cast<unsigned int*>(&(target->leafIndex_)) = size_[0]++;
                else
                    *const_cast<unsigned int*>(&(target->leafIndex_)) = target->son_->leafIndex_;

            }

        }

        // ///////////////////////////////////////////////
        //   Update the list of geometry types present
        // ///////////////////////////////////////////////

        /** \todo This will not work for grids with more than one element type */
        for (int i=0; i<=dim; i++) {

            if (size_[dim-i]>0) {
                myTypes_[i].resize(1);
                myTypes_[i][0] = GeometryType(GeometryType::simplex, dim-i);
            } else
                myTypes_[i].resize(0);

        }

    }
        
        GridImp* grid_;

    // Number of entities per dimension
    int size_[dim+1];

    /** \brief The GeometryTypes present for each codim */
    std::vector<GeometryType> myTypes_[dim+1];

};




template <class GridImp>
class FoamGridGlobalIdSet :
    public IdSet<GridImp,FoamGridGlobalIdSet<GridImp>, unsigned int>
{
            
    public:
        //! constructor stores reference to a grid
        FoamGridGlobalIdSet (const GridImp& g) : grid_(&g) {}
        
        //! define the type used for persistent indices
        typedef unsigned int IdType;
        
        
        //! get id of an entity
        /*
        We use the remove_const to extract the Type from the mutable class,
        because the const class is not instantiated yet.
        */
        template<int cd>
        IdType id (const typename remove_const<GridImp>::type::Traits::template Codim<cd>::Entity& e) const
        {
            return grid_.getRealImplementation(e).globalId();
        }
    
        
        //! get id of subEntity
        /*
            We use the remove_const to extract the Type from the mutable class,
            because the const class is not instantiated yet.
        */
    IdType subId (const typename remove_const<GridImp>::type::Traits::template Codim<0>::Entity& e, int i, int codim) const
        {
            return grid_.getRealImplementation(e).subId(i,codim);
        }

        
        /** \todo Should be private */
        void update() {}

        
        const GridImp* grid_;
};




template<class GridImp>
class FoamGridLocalIdSet :
    public IdSet<GridImp,FoamGridLocalIdSet<GridImp>, unsigned int>
{
        
    public:
        //! define the type used for persistent local ids
        typedef unsigned int IdType;

        
        //! constructor stores reference to a grid
        FoamGridLocalIdSet (const GridImp& g) : grid_(&g) {}
    
        
        //! get id of an entity
        /*
            We use the remove_const to extract the Type from the mutable class,
            because the const class is not instantiated yet.
        */
        template<int cd>
        IdType id (const typename remove_const<GridImp>::type::Traits::template Codim<cd>::Entity& e) const
        {
            // Return id of the host entity
            return grid_.getRealImplementation(e).globalId();
        }
        
        
        //! get id of subEntity
        /*
        * We use the remove_const to extract the Type from the mutable class,
        * because the const class is not instantiated yet.
        */
    IdType subId (const typename remove_const<GridImp>::type::template Codim<0>::Entity& e, int i, int codim) const
        {
            return grid_->getRealImplementation(e).subId(i,codim);
        }
        
        
        /** \todo Should be private */
        void update() {}

        
        const GridImp* grid_;
};


}  // namespace Dune


#endif
