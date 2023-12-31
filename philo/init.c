/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   init.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: dreijans <dreijans@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/12/22 22:44:32 by dreijans      #+#    #+#                 */
/*   Updated: 2023/12/30 20:00:36 by dreijans      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @param data struct containing all general data 
 * for the program
 * @param i index position at moment of init fail
 * @brief allocating memory and filling mutex array.
*/
static void	free_fork_array(t_data *data, int i)
{
	while (i < data->philo_count)
	{
		pthread_mutex_destroy(&data->fork_array[i]);
		i--;
	}
	free(data->fork_array);
}

/**
 * @param data data struct containing all general data 
 * for the program
 * @brief allocating memory and filling mutex array.
*/
static bool	init_fork_array(t_data *data)
{
	int	i;

	i = 0;
	data->fork_array = ft_calloc(sizeof(pthread_mutex_t), data->philo_count);
	if (data->fork_array == NULL)
	{
		write(2, "error creating fork array\n", 27);
		return (false);
	}
	while (i < data->philo_count)
	{
		if (pthread_mutex_init(&data->fork_array[i], NULL) != 0)
		{
			while (i >= 0)
			{
				pthread_mutex_destroy(&data->fork_array[i]);
				write(2, "error creating mutex, mutex destroyed called\n", 46);
				i--;
			}
			free_fork_array(data, i);
			return (false);
		}
		i++;
	}
	return (true);
}

/**
 * @param data data struct containing all general data 
 * for the program
 * @brief initializing data struct mutexes, destroys if failed
*/
static bool	init_data_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->printing, NULL) != 0)
	{
		write(2, "error creating mutex, mutex destroyed called\n", 46);
		return (false);
	}
	if (pthread_mutex_init(&data->eating, NULL) != 0)
	{
		pthread_mutex_destroy(&data->printing);
		write(2, "error creating mutex, mutex destroyed called\n", 46);
		return (false);
	}
	if (pthread_mutex_init(&data->monitor, NULL) != 0)
	{
		pthread_mutex_destroy(&data->eating);
		pthread_mutex_destroy(&data->printing);
		write(2, "error creating mutex, mutex destroyed called\n", 46);
		return (false);
	}
	return (true);
}

/**
 * @param data data struct containing all general data 
 * for the program
 * @brief initializing t_philo_data struct
*/
bool	init_philo(t_data *data)
{
	int		i;

	i = 0;
	data->philo = ft_calloc(sizeof(t_philo), data->philo_count);
	if (data->philo == NULL)
	{
		write(2, "error malloc t_philo struct\n", 29);
		free_all(data);
		return (false);
	}
	while (i < data->philo_count)
	{
		data->philo[i].p_id = i + 1;
		data->philo[i].right_fork = &data->fork_array[i];
		data->philo[i].left_fork = \
			&data->fork_array[(i + 1) % data->philo_count];
		data->philo[i].last_eat = get_current_time();
		data->philo[i].times_eaten = 0;
		data->philo[i].data = data;
		i++;
	}
	return (true);
}

/**
 * @param data data struct containing all general data 
 * for the program
 * @param argc command line argument count
 * @param argv command line argument strings
 * @brief initializing data struct and filling mutex array.
*/
t_data	*init_data_struct(t_data *data, int argc, char **argv)
{
	data = ft_calloc(sizeof(t_data), 1);
	if (data == NULL)
	{
		write(2, "calloc fail\n", 13);
		return (NULL);
	}
	data->start_time = get_current_time();
	data->philo_count = ft_atoi(argv[1]);
	if (init_fork_array(data) == false)
		return (free(data), NULL);
	if (init_data_mutexes(data) == false)
		return (free_init(data), NULL);
	data->stop_monitor = false;
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
	{
		data->eat_count = ft_atoi(argv[5]);
		data->has_eat_count = true;
	}
	return (data);
}
