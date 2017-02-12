<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcusysengpar */

$this->title = 'Create Hcusysengpar';
$this->params['breadcrumbs'][] = ['label' => 'Hcusysengpars', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcusysengpar-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
