// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set ts=8 sw=4 et sts=4:
#ifndef DUNE_FOAMGRID_INTERSECTIONITERATORS_HH
#define DUNE_FOAMGRID_INTERSECTIONITERATORS_HH

#include <dune/foamgrid/foamgrid/foamgridintersections.hh>
#include <dune/foamgrid/foamgrid/foamgridvertex.hh>

#include <map>

/** \file
* \brief The FoamGridLeafIntersectionIterator and FoamGridLevelIntersectionIterator classes
*/

namespace Dune {
    
template<class GridImp>
class FoamGridLevelIntersectionIterator;


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
    
    enum {dimworld=GridImp::dimensionworld};
    
    typedef std::vector<const FoamGridEntityImp<2,dimworld>*> ElementVector;

public:
    
    //! Constructor for a given grid entity and a given neighbor
    FoamGridLeafIntersectionIterator(const FoamGridEntityImp<2,dimworld>* center, int edge) 
        : intersection_(FoamGridLeafIntersection<GridImp>(center,edge))
    {
        if(edge==center->corners())
        {
            // This is the end iterator
            return;
        }
        
        for(std::size_t i=0; i<3; ++i)
            traverseAndPushLeafEdges(center->edges_[i], leafEdges_[i]);
        
        GridImp::getRealImplementation(intersection_).edgeIndex_=edge;
        GridImp::getRealImplementation(intersection_).neighbor_=leafEdges_[edge].begin();
        GridImp::getRealImplementation(intersection_).neighborEnd_=leafEdges_[edge].end();
        if(center->edges_[edge]->elements_.size()==1){
            // This is a boundary edge.
            return;
        }

        // Search for the first intersection.
        // For each edge there is either one or it is a boundary intersection
        topLevelEdgeIter_=leafEdges_.find(edge);
        leafEdgeIter_=topLevelEdgeIter_->second.begin();
        
        GridImp::getRealImplementation(intersection_).neighbor_=leafEdges_[edge].begin();
        GridImp::getRealImplementation(intersection_).neighborEnd_=leafEdges_[edge].end();
        if(GridImp::getRealImplementation(intersection_).neighbor_ != GridImp::getRealImplementation(intersection_).neighborEnd_
           && *GridImp::getRealImplementation(intersection_).neighbor_==center)
        {
            ++GridImp::getRealImplementation(intersection_).neighbor_;
        }
    }

    /** \brief Constructor creating the 'one-after-last'-iterator */
    FoamGridLeafIntersectionIterator(const FoamGridEntityImp<2,dimworld>* center) 
        : intersection_(FoamGridLeafIntersection<GridImp>(center,center->corners()))
    {
        topLevelEdgeIter_=leafEdges_.end();
        
    }

    typedef Dune::Intersection<const GridImp, Dune::FoamGridLeafIntersection> Intersection;

    //! equality
    bool equals(const FoamGridLeafIntersectionIterator<GridImp>& other) const {
        return (GridImp::getRealImplementation(intersection_).center_   == GridImp::getRealImplementation(other.intersection_).center_) 
            && topLevelEdgeIter_ == other.topLevelEdgeIter_
            && *GridImp::getRealImplementation(intersection_).neighbor_ == 
            *GridImp::getRealImplementation(other.intersection_).neighbor_;
    }

    //! prefix increment
    void increment()
    {
        if(topLevelEdgeIter_==leafEdges_.end())
        {
            // This is already the end iterator
            DUNE_THROW(InvalidStateException, "Cannot increment a one past the end iterator");
            return;
        }
        
        ++GridImp::getRealImplementation(intersection_).neighbor_;

        if(GridImp::getRealImplementation(intersection_).neighbor_ ==
           topLevelEdgeIter_->second.end())
        {
            ++leafEdgeIter_;
            if(leafEdgeIter_==topLevelEdgeIter_->second.end())
            {
                ++topLevelEdgeIter_;
                if(topLevelEdgeIter_==leafEdges_.end())
                    return;

                leafEdgeIter_=topLevelEdgeIter_->second.begin();
            }
            
            GridImp::getRealImplementation(intersection_).neighbor_=
                topLevelEdgeIter_->second.begin();
            GridImp::getRealImplementation(intersection_).neighborEnd_=
                topLevelEdgeIter_->second.end();
            if(GridImp::getRealImplementation(intersection_).neighbor_ != GridImp::getRealImplementation(intersection_).neighborEnd_
               && *GridImp::getRealImplementation(intersection_).neighbor_==
               GridImp::getRealImplementation(intersection_).center_)
            {
                ++GridImp::getRealImplementation(intersection_).neighbor_;
            }
        }
    }
    

    //! \brief dereferencing
    const Intersection & dereference() const {
        return reinterpret_cast<const Intersection&>(*this);
    }
private:

    //! \brief Pushes all leaf Edges into leafEdges_
    //!
    //! On returning leafEdges_ will contain the children
    //! of edge that do not have any children.
    //! \param edge The edge whose leafEdge we need.
    //!
    void traverseAndPushLeafEdges(FoamGridEntityImp<1,dimworld>* edge,
                                  ElementVector& leafEdges)
    {
        if(edge->isLeaf())
            leafEdges.insert(leafEdges.end(), edge->elements_.begin(),
                              edge->elements_.end());
        else
        {
            traverseAndPushLeafEdges(edge->sons_[0],leafEdges);
            traverseAndPushLeafEdges(edge->sons_[1],leafEdges);
        }
    }

    mutable MakeableInterfaceObject<Intersection> intersection_;

    //! \brief map from edge index onto the intersections associated with the edge
    std::map<int, ElementVector> leafEdges_;

    typename std::map<int, ElementVector>::const_iterator
    topLevelEdgeIter_;
    typename ElementVector::const_iterator     leafEdgeIter_;
};




//! \todo Please doc me !
template<class GridImp>
class FoamGridLevelIntersectionIterator
{
    
    enum { dim=GridImp::dimension };
    enum { dimworld=GridImp::dimensionworld };

    // Only the codim-0 entity is allowed to call the constructors
    friend class FoamGridEntity<0,dim,GridImp>;

    /** \todo Make this private once FoamGridLeafIntersectionIterator doesn't derive from this class anymore */
protected:
    //! \brief Constructor for a given grid entity and a given neighbor
    //! \param center Pointer to the element where the iterator was created.
    //! \param edge The index of the edge to start the investigation.
    FoamGridLevelIntersectionIterator(const FoamGridEntityImp<2,dimworld>* center, std::size_t edge) 
        : intersection_(FoamGridLevelIntersection<GridImp>(center,edge))
    {
        if(edge==center->corners())
        {
            // This is the end iterator
            GridImp::getRealImplementation(intersection_).neighborIndex_=0;
            return;
        }
        
        if(center->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size()==1){
            // This is a boundary edge.
            GridImp::getRealImplementation(intersection_).neighborIndex_=
                GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size();
            return;
        }
        
        // Search for the first intersection.
        // An intersection has either two neighbor elements on the same level
        // or is a boundary intersection
        while(GridImp::getRealImplementation(intersection_).edgeIndex_ != center->corners()) // not an  end iterator
        {
            GridImp::getRealImplementation(intersection_).neighbor_=GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.begin();
            GridImp::getRealImplementation(intersection_).neighborIndex_=0;
            while(GridImp::getRealImplementation(intersection_).neighbor_!=GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.end() &&
                  (GridImp::getRealImplementation(intersection_).center_==*GridImp::getRealImplementation(intersection_).neighbor_
                   ||GridImp::getRealImplementation(intersection_).center_->level()!=(*GridImp::getRealImplementation(intersection_).neighbor_)->level()))
            {
                ++GridImp::getRealImplementation(intersection_).neighborIndex_;
                ++GridImp::getRealImplementation(intersection_).neighbor_;
            }
            if(GridImp::getRealImplementation(intersection_).neighbor_==GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.end())
            {
                if(GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size()==1)
                {
                    // This is a boundary intersection.
                     GridImp::getRealImplementation(intersection_).neighborIndex_=GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size();
                    return;
                }else
                    // No valid intersection found on this edge, move to next one.
                    ++GridImp::getRealImplementation(intersection_).edgeIndex_;
            }else
                // intersection with another element found!
                break;
        }
        if(GridImp::getRealImplementation(this->intersection_).edgeIndex_==center->corners())
        {
            // This is the end iterator
            GridImp::getRealImplementation(this->intersection_).neighborIndex_=0;
        }
    }

    /** \brief Constructor creating the 'one-after-last'-iterator */
    FoamGridLevelIntersectionIterator(const FoamGridEntityImp<2,dimworld>* center) 
        : intersection_(FoamGridLevelIntersection<GridImp>(center,center->corners()))
    {}

public:

    typedef Dune::Intersection<const GridImp, Dune::FoamGridLevelIntersection> Intersection;

  //! equality
  bool equals(const FoamGridLevelIntersectionIterator<GridImp>& other) const {
      return (GridImp::getRealImplementation(this->intersection_).center_   == GridImp::getRealImplementation(other.intersection_).center_) 
          && (GridImp::getRealImplementation(this->intersection_).edgeIndex_ == GridImp::getRealImplementation(other.intersection_).edgeIndex_)
          && (GridImp::getRealImplementation(this->intersection_).neighborIndex_ == GridImp::getRealImplementation(other.intersection_).neighborIndex_);
  }

    //! prefix increment
    void increment() {
        if(GridImp::getRealImplementation(intersection_).edgeIndex_==
           GridImp::getRealImplementation(intersection_).center_->corners())
        {
            // This is already the end iterator
            GridImp::getRealImplementation(intersection_).neighborIndex_=0;
            return;
        }
        if(GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size()==1)
        {
            // This was a boundary intersection.
            ++GridImp::getRealImplementation(intersection_).edgeIndex_;
            if(GridImp::getRealImplementation(intersection_).edgeIndex_ < GridImp::getRealImplementation(intersection_).center_->corners()){
                // There is another edge, initialize neighbor_ iterator.
                GridImp::getRealImplementation(intersection_).neighbor_=GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.begin();
                GridImp::getRealImplementation(intersection_).neighborIndex_=0;
            }
        }
        // Search for the first intersection.
        // An intersection has either two neighbor elements on the same level
        // or is a boundary intersection
        while(GridImp::getRealImplementation(intersection_).edgeIndex_ != GridImp::getRealImplementation(intersection_).center_->corners()) // still a valid edge
        {
            while(GridImp::getRealImplementation(intersection_).neighbor_!=GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.end() &&
                  (GridImp::getRealImplementation(intersection_).center_==*GridImp::getRealImplementation(intersection_).neighbor_
                   ||GridImp::getRealImplementation(intersection_).center_->level()!=(*GridImp::getRealImplementation(intersection_).neighbor_)->level()))
            {
                ++GridImp::getRealImplementation(intersection_).neighborIndex_;
                ++GridImp::getRealImplementation(intersection_).neighbor_;
            }
            if(GridImp::getRealImplementation(intersection_).neighbor_==
               GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.end())
            {
                if(GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size()==1)
                {
                    // This is a boundary intersection.
                    GridImp::getRealImplementation(intersection_).neighborIndex_=
                        GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.size();
                    return;
                }
                else
                {
                    // No valid intersection found on this edge, move to next one.
                    ++GridImp::getRealImplementation(intersection_).edgeIndex_;
                    if(GridImp::getRealImplementation(intersection_).edgeIndex_ < GridImp::getRealImplementation(intersection_).center_->corners())
                    {
                        // There is another edge, initialize neighbor_ iterator.
                        GridImp::getRealImplementation(intersection_).neighbor_=GridImp::getRealImplementation(intersection_).center_->edges_[GridImp::getRealImplementation(intersection_).edgeIndex_]->elements_.begin();
                        GridImp::getRealImplementation(intersection_).neighborIndex_=0;
                    }
                }
            }else
                // intersection with another element found!
                break;
        }
        if(GridImp::getRealImplementation(intersection_).edgeIndex_==
           GridImp::getRealImplementation(intersection_).center_->corners())
        {
            // This is the end iterator
            GridImp::getRealImplementation(intersection_).neighborIndex_=0;
        }
    }
        

    //! \brief dereferencing
    const Intersection & dereference() const
    {
        return reinterpret_cast<const Intersection&>(intersection_);
    }

private:
  //**********************************************************
  //  private data
  //**********************************************************

    /** \brief The actual intersection
    */
    mutable MakeableInterfaceObject<Intersection> intersection_;

};


}  // namespace Dune

#endif
