/**
 * This module defines a generic network co-evolution process.
 *
 * The function "evolve" takes a multilayer network as input and at every step
 * updates each of its layers taking one of the following actions:
 * 1) no action (the layer remains unchanged - used to set different evolution speeds)
 * 2) internal evolution (the layer evolves according to some internal dynamics, defined by an EvolutionModel)
 * 3) external evolution (the layer imports nodes and edges from another layer)
 */

#ifndef GENERATION_H_
#define GENERATION_H_

#include "datastructures.h"

namespace mlnet {

typedef int evolution_strategy;

const int EVOLUTION_DEGREE=0;

/**********************************************************************/
/** edge closure strategy *********************************************/
/**********************************************************************/
NodeSharedPtr choice_uniform(MLNetworkSharedPtr mnet, LayerSharedPtr layer);
NodeSharedPtr choice_common_friends(MLNetworkSharedPtr mnet, LayerSharedPtr layer, LayerSharedPtr vertex);
NodeSharedPtr choice_degree(MLNetworkSharedPtr mnet, LayerSharedPtr layer);


/**********************************************************************/
/** Evolution models **************************************************/
/**********************************************************************/
class EvolutionModel {
public:
    virtual ~EvolutionModel() = 0;
    virtual void init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors) = 0;
    virtual void internal_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors) = 0;
    virtual void external_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& target_layer, ActorListSharedPtr& available_actors, const LayerSharedPtr& ext_layer) = 0;
};

typedef std::shared_ptr<EvolutionModel> EvolutionModelSharedPtr;

/**
 * @brief Grows a network by first creating a complete graph with m0 nodes, then adding a new node at a time and connecting it to m other nodes chosen with a probability proportional to their degree.
 **/
class BAEvolutionModel : public EvolutionModel {
	size_t m0, m;
public:
	BAEvolutionModel(size_t m0, size_t m);
	~BAEvolutionModel();
	void init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors);
    void internal_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors);
    void external_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& target_layer, ActorListSharedPtr& available_actors, const LayerSharedPtr& ext_layer);
};

/**
 * @brief Grows a network by first creating all the nodes and then at every step choosing two nodes (uniform probability) to connect with an edge.
 **/
class UniformEvolutionModel : public EvolutionModel {
	size_t m0;
public:
	UniformEvolutionModel(size_t m0);
    ~UniformEvolutionModel();
    void init_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors);
    void internal_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& layer, ActorListSharedPtr& available_actors);
    void external_evolution_step(MLNetworkSharedPtr& mnet, const LayerSharedPtr& target_layer, ActorListSharedPtr& available_actors, const LayerSharedPtr& ext_layer);
};

/**********************************************************************/
/** Evolution method **************************************************/
/**********************************************************************/
/**
 * @brief Grows the input multilayer network.
 * @param mnet MLNetwork to grow
 * @param num_of_steps number of evolution steps
 * @param pr_no_event[] for each layer, the probability that during an evolution step the layer does not change
 * @param pr_internal_event[] for each layer, the probability that if something happens it is an internal evolution according to the evolution_model[] parameter
 * @param dependency[][] The (i,j) element of this matrix indicates the probability that, given an external evolution event, layer i will consider layer j as a potential candidate to import edges from
 * @param evolution_model[] for each layer, a specification of how the layer should evolve when an internal event happens
 **/
MLNetworkSharedPtr evolve(
		long num_of_steps,
		size_t num_actors,
		const vector<double>& pr_no_event,
		const vector<double>& pr_internal_event,
		const matrix<double>& dependency,
		const vector<EvolutionModelSharedPtr>& evolution_model);

} // namespace

#endif /* GENERATION_H_ */
