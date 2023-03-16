/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmatsuo <mmatsuo@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 20:09:50 by mmatsuo           #+#    #+#             */
/*   Updated: 2023/03/07 18:30:39 by mmatsuo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

bool	is_philo_starve(t_data *arg)
{
	int	i;

	i = 0;
	while (i < arg->num_philo)
	{
		pthread_mutex_lock(&arg->philo_mtx[i]);
		if (arg->time_to_die < get_time() - arg->philo[i].time_last_eat)
		{
			pthread_mutex_unlock(&arg->philo_mtx[i]);
			arg->dead_num = i;
			return (true);
		}
		pthread_mutex_unlock(&arg->philo_mtx[i]);
		i++;
	}
	return (false);
}

bool	is_num_of_eat_reached(t_data *arg)
{
	int			i;
	long		cnt;

	cnt = 0;
	i = 0;
	while (i < arg->num_philo)
	{
		pthread_mutex_lock(&arg->philo_mtx[i]);
		if (arg->philo_must_eat <= arg->philo[i].num_of_eaten)
		{
			pthread_mutex_unlock(&arg->philo_mtx[i]);
			cnt++;
		}
		pthread_mutex_unlock(&arg->philo_mtx[i]);
		i++;
	}
	if (cnt == arg->num_philo)
		return (true);
	return (false);
}

void	*monitor(void *arg_void)
{
	t_data	*arg;

	arg = arg_void;
	while (1)
	{
		if (is_philo_starve(arg))
		{
			pthread_mutex_lock(&arg->write_finish_mtx);
			printf("%ld %d %s\n",
				calc_elapsed_time(&arg->philo[arg->dead_num].time_start),
				arg->dead_num + 1, STATE_DIED);
			arg->is_finish = true;
			pthread_mutex_unlock(&arg->write_finish_mtx);
			break ;
		}
		if (is_num_of_eat_reached(arg))
		{
			pthread_mutex_lock(&arg->write_finish_mtx);
			arg->is_finish = true;
			pthread_mutex_unlock(&arg->write_finish_mtx);
			break ;
		}
	}
	return (NULL);
}
