<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuwinddirdatainfo */

$this->title = 'Create Hcuwinddirdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuwinddirdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuwinddirdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
